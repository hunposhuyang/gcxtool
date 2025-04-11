#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "gcx.h"

struct MemoryOperation {
    uintptr_t address;
    std::string content;
};

class TXT
{
public:
    std::string stringFilePath = "";
    uintptr_t base_offset = 0;

    TXT(const char* strfilepath, const char* baseOffset);
    ~TXT();
    void WriteStringToGcxdata(Gcx &gcx);
    std::vector<MemoryOperation> process_file(std::string& filename, uintptr_t base_address);


};

TXT::TXT(const char* strfilepath, const char* baseOffset)
{
    this->stringFilePath = strfilepath;
	this->base_offset = std::stoull(baseOffset, nullptr, 0);
}

TXT::~TXT()
{
    std::cout << "TXT destructor called" << std::endl;
}

void TXT::WriteStringToGcxdata(Gcx &gcx)
{
    std::vector<MemoryOperation> operations = process_file(stringFilePath, base_offset);

    for (const auto& op : operations) {
        uintptr_t relative_address = op.address;

        // 计算目标地址
        uint8_t* target = gcx.data + relative_address;

        // 将字符串内容写入目标地址
        const char* content = op.content.c_str();
        size_t length = op.content.size();

        // 写入内容并添加字符串结束符
        for (size_t i = 0; i <= length; ++i) { // 包含 '\0'
            if (i < length) {
                *target++ = static_cast<uint8_t>(content[i]);
            } else {
                *target++ = '\0'; // 确保字符串结束符被写入
            }
        }
    }
}




std::vector<MemoryOperation> TXT::process_file(std::string& stringFile, uintptr_t base_address) {
    std::ifstream file(stringFile);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + stringFile);
    }

    std::vector<MemoryOperation> operations;
    std::string line;

    while (std::getline(file, line)) {
        const size_t tab_pos = line.find('\t');
        if (tab_pos == std::string::npos) continue;

        const std::string offset_str = line.substr(0, tab_pos);
        std::string content = line.substr(tab_pos + 1);

        try {
            uintptr_t offset = std::stoull(offset_str, nullptr, 0);
            operations.push_back({
                base_address + offset,
                content + '\0'  // 添加字符串结束符
                });
        }
        catch (const std::invalid_argument&) {
            continue;
        }
        catch (const std::out_of_range&) {
            continue;
        }
    }

    file.close();
    return operations;
}