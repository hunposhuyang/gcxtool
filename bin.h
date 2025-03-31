#pragma once
#include <fstream>
#include <filesystem>
#include<cstdint>
#include "gcx.h"


class Bin
{


public:
    Bin(std::string filename);
    ~Bin();

    uint32_t sign = 0x48505359;
    std::string filename = "scenerio.bin";
    uint8_t* binData;
    uint32_t binSize;

    void initData();
    void encryptBuffer(uint32_t seed, uint8_t* src, int size);
    void encryptBinFile(uint32_t seed);
    
};
