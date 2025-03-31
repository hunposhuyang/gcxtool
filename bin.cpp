#include"bin.h"

Bin::Bin(std::string filename) {
	this->filename = filename;
	initData();
}

Bin::~Bin() {
    delete[] binData;

}

void Bin::initData() {

    std::ifstream fs;
	uintmax_t fileSize = std::filesystem::file_size(filename);

	if (fileSize > static_cast<uintmax_t>(std::numeric_limits<int>::max())) {
		throw std::overflow_error("File size exceeds maximum int value");
	}

	int size = static_cast<int>(fileSize);

	fs.open(filename, std::ios::binary);
	uint8_t* data = new uint8_t[size];
	fs.read((char*)data, size);
	this->binData = data;
    this->binSize = size;
	fs.close();

}

void Bin::encryptBinFile(uint32_t seed)
{
	Bin::encryptBuffer(seed, binData, binSize);
}

void Bin::encryptBuffer(uint32_t seed, uint8_t* src, int size) {
	for (int i = 0; i < size; i++) {
		seed = (seed * 0x7D2B89DD) + 0xCF9;
		int c = (seed >> 0xF) & 0x00FF;
		src[i] ^= c;
	}
}


