#pragma once
#include <fstream>
#include <filesystem>


typedef uint8_t* GcxProc;

struct GcxBlockHeader {
	uint32_t procOffset;
	uint32_t resourceTableOffset;
	uint32_t stringResourcesOffset;
	uint32_t fontDataOffset;
	uint32_t seed;
};

class Gcx {
public:
	Gcx(std::string filename);
	~Gcx();

	void open();
    int getNumProc() const;
    GcxProc getMainProc();
	int getNumResource() const;
	int getResourcesSize();
	GcxProc getProc(int idx);
	int getResourceSize(int idx);
	GcxProc getResource(int idx);
	bool isScriptResource(int idx);
	void outStringTableData(std::string& binFilename, std::string& output);
	void outMergedData(std::string& binFilename, std::string& output);
	void decryptStringResources();
	void decodeBuffer(uint32_t seed, uint8_t* src, int size);
	void setProc();
	void initData();
	void setNumProc();
	void setNumResource();


	std::string filename = "scenerio.scx";
	uint32_t Size;
	int numProc = 0;
	uint8_t* Data;
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
};

