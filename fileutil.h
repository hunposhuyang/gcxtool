#pragma once
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>


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

// 定义 copyFile 函数
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
std::string getCurrentDir(std::string& output) {
	std::filesystem::path p{ output };
	return p.filename().u8string();
}

inline
std::string getExtension(const std::string& output) {
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
void writeDataToFile(uint8_t* data, int size, const std::string& filename, std::string& output) {

	if (!std::filesystem::exists(output))
		std::filesystem::create_directories(output);

	updateDir(filename, output);

	std::ofstream ofs(output, std::ofstream::app | std::ofstream::binary);
	ofs.write((char*)data, size);
	//ofs << "HPSY"; 
	ofs.close();

}

inline
void writecryptDataToFile(uint8_t* data, int size, const std::string& filename, std::string& output) {
	createDirectory(output);
	updateDir(filename, output);
	std::ofstream ofs(output, std::ofstream::binary);
	ofs.write((char*)data, size);
	ofs.close();
	resetDir(output);
}


