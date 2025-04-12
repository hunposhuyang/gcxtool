#pragma once
#include <sstream>
#include <fstream>
#include <iostream>
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#error "No filesystem support"
#endif

inline
bool copyFile(const std::string& src, const std::string& dest) {
    std::ifstream srcFile(src, std::ios::binary);
    if (!srcFile) {
        std::cerr << "Source file not found: " << src << std::endl;
        return false;
    }

    std::ofstream destFile(dest, std::ios::binary);
    if (!destFile) {
        std::cerr << "Failed to create destination file: " << dest << std::endl;
        return false;
    }

    destFile << srcFile.rdbuf();
    return true;
}

inline
std::string getCurrentDir(std::string& output) {
	std::filesystem::path p{ output };
	return p.filename().u8string();
}

inline
std::string getFileDirName(std::string& output) {
	std::filesystem::path p{ output };
	return p.parent_path().filename().u8string();
}

inline
std::string getExtension(const std::string& output) {
	std::filesystem::path p{ output };
	return p.extension().u8string();
}



inline
void updateDir(const std::string& path, std::string& output) {
	std::filesystem::path p{ output };
	p.append(path);
	output = p.u8string();
}

inline
void resetDir(std::string& output) {
	std::filesystem::path p{ output };
	output = p.parent_path().u8string();
}


inline
std::string getExtension(std::string& output) {
	std::filesystem::path p{ output };
	return p.extension().u8string();
}

inline
std::string getExtensionlessName(const std::string& output) {
	std::filesystem::path p{ output };
	return p.stem().u8string();
}

inline
bool isDirectory(std::string& output) {
	std::filesystem::path p{ output };
	return std::filesystem::is_directory(p);
}

inline
bool fileExists(std::string& output) {
	std::filesystem::path p{ output };
	return std::filesystem::exists(p);
}

inline
int64_t getFileSize(const std::string& input) {
	return std::filesystem::file_size(input);
}

inline
int64_t getAlignment(int64_t currentOffset, int64_t alignSize) {
	uint64_t step = (alignSize - (currentOffset % alignSize));
	if (step != alignSize)
		return step;
	return 0;
}

inline
bool filenameContainsString(const std::string& output, const std::string& string) {
	std::string filename = getExtensionlessName(output);

	if (filename.find(string) != std::string::npos)
		return true;

	return false;
}

inline
void bufferFromFile(const std::string& filename, uint8_t* dst, int size) {
	std::ifstream fs;
	size = std::filesystem::file_size(filename);

	fs.open(filename, std::ios::binary);
	fs.read((char*)dst, size);
	fs.close();
}

inline
uint8_t* bufferFromFile(const  std::string& filename, int& size, int pad = 0) {
	std::ifstream fs;
	size = std::filesystem::file_size(filename);

	fs.open(filename, std::ios::binary);
	uint8_t* p = new uint8_t[size + pad]();
	fs.read((char*)p, size);
	fs.close();

	return p;
}

inline
void createDirectory(std::string& output) {
	if (output == "") return;
	if (std::filesystem::exists(output)) return;
	std::filesystem::create_directories(output);
}

inline
void appendDataToFile(uint8_t* data, int size, const std::string& filename, std::string& output) {
	createDirectory(output);
	updateDir(filename, output);
	std::ofstream ofs(output, std::ofstream::binary | std::ofstream::app);
	ofs.write((char*)data, size);
	ofs.close();
	resetDir(output);
}

inline
void appendStringToFile(const std::string& string, const std::string& filename, std::string& output) {
	int strsize = string.size() + 1;
	appendDataToFile((uint8_t*)string.c_str(), strsize, filename, output);
}

inline
void appendPaddingToFile(int padSize, const std::string& filename, std::string& output) {
	if (!padSize) return;
	uint8_t* pad = new uint8_t[padSize]();
	appendDataToFile(pad, padSize, filename, output);
	delete[] pad;
}

inline
void writeDataToFile(uint8_t* data, int size, const std::string& filename, std::string& output) {
	createDirectory(output);
	updateDir(filename, output);
	std::ofstream ofs(output, std::ofstream::binary);
	ofs.write((char*)data, size);
	ofs.close();
	resetDir(output);
}

inline
void genericSwap(void* data, int numData, int length) {

	for (int i = 0; i < numData; i++) {

		switch (length) {
		case 2: {
			uint16_t* pData = (uint16_t*)data;
			pData[i] = _byteswap_ushort(pData[i]);
			break;
		}
		case 4: {
			uint32_t* pData = (uint32_t*)data;
			pData[i] = _byteswap_ulong(pData[i]);
			break;
		}
		case 8: {
			uint64_t* pData = (uint64_t*)data;
			pData[i] = _byteswap_uint64(pData[i]);
			break;
		}
		}
	}

}

