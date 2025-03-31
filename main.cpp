#include<stdio.h>
#include "gcx.h"
#include "fileutil.h"
#include"crypto.h"
#include <string>
#include <cstring>
bool checkKeySet(const char* Key) {
	if (Key == NULL) {
		printf("Key not set, set key with -k. Usually this is the path to where the file resides");
		return 0;
	}
	return 1;
}


bool checkFileExists(const char* filename) {
	std::string input = filename;

	if (!fileExists(input)) {
		printf("Input file %s, not found", filename);
		return 0;
	}

	return 1;
}

void cliCrypt(bool mode,const char* gcxFilepath, const char* Key) {
    std::string input = gcxFilepath;
    std::string outputFile = gcxFilepath;
    resetDir(outputFile);

    if (!checkFileExists(gcxFilepath)) return;
    if (!checkKeySet(Key)) return;


    Crypto crypto(Key);

    if (mode) {
        crypto.encrypt(input, outputFile);
    }
    else {
        // 创建一个 gcxFile 的备份
        std::string backupFile = input + ".bak"; // 生成备份文件名
        if (!copyFile(input, backupFile)) {      // 调用 copyFile 函数复制文件
            printf("Failed to create backup file: %s\n", backupFile.c_str());
            return;
        }
        crypto.decrypt(input, outputFile);
    }
}

void exportResources(Gcx& gcx, const std::string& input, std::string& output) {

	std::string foldername = getExtensionlessName(input) + "_strres";
	updateDir(foldername, output);

	std::string binFilename = "scenerio.bin";
	gcx.outStringTableData(binFilename, output);
    resetDir(output);

}

void exportMergeData(Gcx& gcx, const std::string& input, std::string& output) {

    std::string foldername = getExtensionlessName(input) + "_merge";
    updateDir(foldername, output);

    std::string Filename = "scenerio.gcx";
    gcx.outMergedData(Filename, output);

}



    const char* Create(const char* gcxFile, const char* binFile)
    {
        std::string output = "";
        Gcx gcx = Gcx(gcxFile);
        Gcx bin = Gcx(binFile);
        gcx.open();
        uint32_t seed = gcx.blockHeader->seed;
        bin.decodeBuffer(seed, bin.Data, bin.Size);

        memcpy(gcx.blockStart + gcx.blockHeader->stringResourcesOffset, bin.Data, gcx.getResourcesSize());
        exportMergeData(gcx, gcxFile, output);
    
        // 分配内存并使用 strcpy_s 安全复制字符串
        char* result = new char[output.size() + 1];
        strcpy_s(result, output.size() + 1, output.c_str()); // 修复：指定缓冲区大小
        return result;
    }

void Extract(const char* gcxFile)
{
    std::string output = "";
    Gcx gcx = Gcx(gcxFile);
    gcx.open();
    gcx.decryptStringResources();
    exportResources(gcx, gcxFile,output);
}



void useage()
{
    printf("CreateScenerio:-c gcx_file_path bin_file_path\n");
    printf("ExtractScenerio:-e .gcx_file_path\n");
    printf("psn's addtion:otherArgument -k Key (ex:stage/init)\n");
}

int main(int argc, const char* argv[])
{
    // 判断命令行参数
    if (argc > 3 && strcmp(argv[1], "-c") == 0)
    {
        const char*filepath = Create(argv[2], argv[3]);
        if(argc == 6 && strcmp(argv[4], "-k") == 0)
        {
            cliCrypt(1, filepath, argv[5]);
        }
        delete[] filepath;
    }
    else if (argc > 2 && strcmp(argv[1], "-e") == 0)
    {
        if(argc == 5 && strcmp(argv[3], "-k") == 0)
        {
            cliCrypt(0, argv[2], argv[4]);
        }
        Extract(argv[2]);

    }
    else
    {
        useage();
    }

    return 0;
}