#pragma once
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>


typedef uint8_t* GcxProc;

struct GcxBlockHeader {
	uint32_t procOffset;
	uint32_t resourceTableOffset;
	uint32_t stringDataOffset;
	uint32_t fontDataOffset;
	uint32_t seed;
};

struct StringTable {
    uint32_t offset; // 字符串的偏移量
    std::string string; // 字符串内容
};

class Gcx {
public:
	Gcx(std::string filename);
	~Gcx();
	void open();
	void initData();
	void CreateOffsetStringMap();

	uint8_t* data;
	std::vector<StringTable> stringTables; // 存储字符串表的动态数组
	std::string gcxfilepath = "scenerio.gcx";
	void DecryptStringData();
    void save(const std::string& filename);

private:

	uint32_t size;
	int numProc = 0;
	uint8_t* procStart;
	int32_t* procTable;
	int numResource = 0;
	uint32_t* timestamp;
	uint8_t* blockStart;
	uint32_t* mainProcSize;
	uint8_t* mainProcStart;
	uint32_t* resourceTable;
	GcxBlockHeader* blockHeader;
	uint32_t blockHeaderOffset;

	GcxProc getProc(int idx);
	GcxProc getMainProc();
	GcxProc getResource(int idx);

	int getNumProc() const;
	int getNumResource() const;
	int getResourcesSize();
	int getResourceSize(int idx);

	void setNumProc();
	void setNumResource();
	bool isScriptResource(int idx);
	void CryptBuffer(uint32_t seed, uint8_t* src, int size);
	void setProc();
	bool isValidString(const std::string& str);


};

