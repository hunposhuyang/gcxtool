#include "gcx.h"
#include <filesystem> // 需要 C++17 支持
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem; // 命名空间别名

Gcx::Gcx(std::string filename) {
	this->gcxfilepath = filename;
	initData();
}

void Gcx::initData() {
	std::ifstream fs;
	uintmax_t fileSize = std::filesystem::file_size(gcxfilepath);

	if (fileSize > static_cast<uintmax_t>(std::numeric_limits<int>::max())) {
		throw std::overflow_error("File size exceeds maximum int value");
	}

	int size = static_cast<int>(fileSize);

	fs.open(gcxfilepath, std::ios::binary);
	uint8_t* data = new uint8_t[size];
	fs.read((char*)data, size);
	this->data = data;
	this->size = size;
	fs.close();
}

Gcx::~Gcx() {
	delete[] data;
}

int Gcx::getNumProc() const {
	return numProc;
}

void Gcx::setNumProc() {
	int i = 0;
	while (procTable[i] != -1) i++;
	numProc = i;
}


bool Gcx::isScriptResource(int idx) {
	return (resourceTable[idx] & 0xFF000000) != 0x80000000;
}

int Gcx::getResourceSize(int idx) {
	int offset = resourceTable[idx] & 0x00FFFFFF;
	int nextOffset = idx == numResource ? blockHeader->fontDataOffset : resourceTable[idx + 1] & 0x00FFFFFF;

	return nextOffset - offset;
}

int Gcx::getResourcesSize() {
	return blockHeader->fontDataOffset - blockHeader->stringDataOffset;
}

int Gcx::getNumResource() const {
	return numResource;
}

void Gcx::setNumResource() {
	numResource = (blockHeader->stringDataOffset - blockHeader->resourceTableOffset) / 4;
}

GcxProc Gcx::getResource(int idx) {
	uint8_t* resources = &blockStart[blockHeader->stringDataOffset];
	return &resources[resourceTable[idx] & 0x00FFFFFF];
}

void Gcx::CryptBuffer(uint32_t seed, uint8_t* src, int size) {
	for (int i = 0; i < size; i++) {
		seed = (seed * 0x7D2B89DD) + 0xCF9;
		int c = (seed >> 0xF) & 0x00FF;
		src[i] ^= c;
	}
}

void Gcx::CryptStringData() {
	if (!blockHeader->seed) return;
	int size = blockHeader->fontDataOffset - blockHeader->stringDataOffset;
	uint8_t* stringTableData = &blockStart[blockHeader->stringDataOffset];
	if (!size) return;
	CryptBuffer(blockHeader->seed, stringTableData, size);
	blockHeader->seed = 0;
}

GcxProc Gcx::getMainProc() {
	return mainProcStart;
}

GcxProc Gcx::getProc(int idx) {
	return &procStart[procTable[idx] & 0x00FFFFFF];
}

void Gcx::setProc() {
	uint32_t mainProcOffset = *(uint32_t*)&blockStart[blockHeader->procOffset];

	procStart = &blockStart[blockHeader->procOffset + 4];
	mainProcSize = (uint32_t*)&procStart[mainProcOffset];
	mainProcStart = &procStart[mainProcOffset + 4];
}

void Gcx::open() {
	timestamp = (uint32_t*)data;
	procTable = (int32_t*)&data[4];
	setNumProc();

	blockStart  = (uint8_t*)(&procTable[numProc + 1]);
	blockHeader = (GcxBlockHeader*)blockStart;
	
	resourceTable = (uint32_t*)&blockStart[blockHeader->resourceTableOffset];	
	setNumResource();
	CryptStringData();
	setProc();
}

bool isValidUTF8(const std::string& str) {
    const unsigned char* data = (const unsigned char*)str.data();
    size_t len = str.size();
    size_t i = 0;
    while (i < len) {
        unsigned char c = data[i];
        if (c < 0x80) {
            i++;
        } else if ((c & 0xE0) == 0xC0) { // 2字节
            if (i + 1 >= len || (data[i+1] & 0xC0) != 0x80) return false;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3字节
            if (i + 2 >= len || 
                (data[i+1] & 0xC0) != 0x80 || 
                (data[i+2] & 0xC0) != 0x80) return false;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4字节
            if (i + 3 >= len || 
                (data[i+1] & 0xC0) != 0x80 || 
                (data[i+2] & 0xC0) != 0x80 || 
                (data[i+3] & 0xC0) != 0x80) return false;
            i += 4;
        } else {
            return false; // 非法起始字节
        }
    }
    return true;
}

bool Gcx::isValidString(const std::string& str) {
    if (str.empty()) return false;

    // 首先验证整个字符串是有效的UTF-8
    if (!isValidUTF8(str)) return false;

    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        if (c < 0x80) { // ASCII字符
            // 排除控制字符（0x00-0x1F）和DEL（0x7F）
            if (c < 0x20 || c == 0x7F) return false;
            i++;
        } else { // 非ASCII字符（UTF-8多字节）
            // 已通过isValidUTF8验证，直接跳过该字符
            int len = 0;
            if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            else return false; // 不应触发，因已验证UTF-8有效性
            i += len;
        }
    }
    return true;
}

void Gcx::CreateOffsetStringMap() {
    size_t startOffset = blockHeader->stringDataOffset; // 初始化起始偏移量

    for (size_t i = blockHeader->stringDataOffset; i < blockHeader->fontDataOffset; ++i) {
        if (data[i] == 0x00) { // 遇到 0x00
            // 提取从 startOffset 到 i 的子数组作为字符串
            std::string str(reinterpret_cast<const char*>(&data[startOffset]), i - startOffset);

            if (isValidString(str)) {
                // 将字符串及其偏移量存储到 stringTables
                stringTables.push_back({ static_cast<uint32_t>(startOffset), str });
            }

            // 更新起始偏移量
            startOffset = i + 1;
        }
    }

    // 处理末尾没有 0x00 的情况
    if (startOffset < blockHeader->fontDataOffset) {
        std::string str(reinterpret_cast<const char*>(&data[startOffset]), blockHeader->fontDataOffset - startOffset);
        if (isValidString(str)) {
            stringTables.push_back({ static_cast<uint32_t>(startOffset), str });
        }
    }

}

void Gcx::save(const std::string& filename) {
    // 1. 检查文件是否存在，如果存在则备份
    if (fs::exists(filename)) {
        const std::string backupName = filename + "_write.bak";
        if (fs::exists(backupName)) {
            fs::remove(backupName); // 删除旧备份
        }
        fs::rename(filename, backupName); // 创建新备份
    }

    // 2. 写入新数据到文件
    std::ofstream fs(filename, std::ios::binary);
    if (!fs) {
        throw std::runtime_error("无法打开文件进行写入");
    }
    fs.write(reinterpret_cast<const char*>(data), size); // 假设 data 是 const
    // fs.close(); // 可省略，析构时自动关闭
}
