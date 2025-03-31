#include "gcx.h"
#include "fileutil.h"

Gcx::Gcx(std::string filename) {
	this->filename = filename;
	initData();
}

void Gcx::initData() {
	std::ifstream fs;
	uintmax_t fileSize = std::filesystem::file_size(filename);

	if (fileSize > static_cast<uintmax_t>(std::numeric_limits<int>::max())) {
		throw std::overflow_error("File size exceeds maximum int value");
	}

	int size = static_cast<int>(fileSize);

	fs.open(filename, std::ios::binary);
	uint8_t* data = new uint8_t[size];
	fs.read((char*)data, size);
	this->Data = data;
	this->Size = size;
	fs.close();
}

Gcx::~Gcx() {
	delete[] Data;
}

int Gcx::getNumProc() const {
	return numProc;
}

void Gcx::setNumProc() {
	int i = 0;
	while (procTable[i] != -1) i++;
	numProc = i;
}

void Gcx::outStringTableData(std::string& binFilename, std::string& output) {
	int size = blockHeader->fontDataOffset - blockHeader->stringResourcesOffset;
	uint8_t* stringTableData = &blockStart[blockHeader->stringResourcesOffset];
	if (!size) return;
	writeDataToFile(stringTableData, size, binFilename, output);
}

void Gcx::outMergedData(std::string& Filename, std::string& output) {
	int size = this->Size;
	if (!size) return;
	writeDataToFile(this->Data, size, Filename, output);
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
	return blockHeader->fontDataOffset - blockHeader->stringResourcesOffset;
}

int Gcx::getNumResource() const {
	return numResource;
}

void Gcx::setNumResource() {
	numResource = (blockHeader->stringResourcesOffset - blockHeader->resourceTableOffset) / 4;
}

GcxProc Gcx::getResource(int idx) {
	uint8_t* resources = &blockStart[blockHeader->stringResourcesOffset];
	return &resources[resourceTable[idx] & 0x00FFFFFF];
}

void Gcx::decodeBuffer(uint32_t seed, uint8_t* src, int size) {
	for (int i = 0; i < size; i++) {
		seed = (seed * 0x7D2B89DD) + 0xCF9;
		int c = (seed >> 0xF) & 0x00FF;
		src[i] ^= c;
	}
}

void Gcx::decryptStringResources() {
	if (!blockHeader->seed) return;
	int size = blockHeader->fontDataOffset - blockHeader->stringResourcesOffset;
	uint8_t* stringTableData = &blockStart[blockHeader->stringResourcesOffset];
	if (!size) return;
	decodeBuffer(blockHeader->seed, stringTableData, size);
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
	timestamp = (uint32_t*)Data;
	procTable = (int32_t*)&Data[4];
	setNumProc();

	blockStart  = (uint8_t*)(&procTable[numProc + 1]);
	blockHeader = (GcxBlockHeader*)blockStart;
	
	resourceTable = (uint32_t*)&blockStart[blockHeader->resourceTableOffset];	
	setNumResource();
	setProc();
}


