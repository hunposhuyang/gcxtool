#include <iostream>
#include "fileutil.h"
#include "crypto.h"
#include "gcx.h"
#include "string.h"
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

void FileCrypt(bool mode,const char* gcxFilepath, const char* Key) {
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
        std::string backupFile = input + "_enc.bak"; // 生成备份文件名
        if (!copyFile(input, backupFile)) {      // 调用 copyFile 函数复制文件
            printf("Failed to create backup file: %s\n", backupFile.c_str());
            return;
        }
        crypto.decrypt(input, outputFile);
    }
}

void ExtractStringData(Gcx &gcx){


    if (!isDirectory(gcx.gcxfilepath)) {
        std::cout << "Not a directory" << std::endl;
        return;
    }
    gcx.CreateOffsetStringMap();
    // 创建一个文件并将字符串表写入文件,编码为utf-8
    std::string output = getExtensionlessName(gcx.gcxfilepath) + "_" + getCurrentDir(gcx.gcxfilepath)+".txt";
    std::ofstream offsetFile(output);
    if (!offsetFile.is_open()) {
        std::cerr << "Failed to create offset file" << std::endl;
        return;
    }

    // 写入 UTF-8 BOM (EF BB BF)
    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    offsetFile.write(reinterpret_cast<char*>(bom), sizeof(bom));

    // 设置输出格式为十六进制，前面补0，8位宽度
    offsetFile << std::hex << std::setfill('0');

    for (const auto& table : gcx.stringTables) {
        offsetFile << "0x" << std::setw(8) << table.offset << "\t" << table.string << std::endl;
    }

    offsetFile.close();
    std::cout << output << " created successfully." << std::endl;

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [-x|-w] [options]..." << std::endl;
        return 1;
    }

    if (std::string(argv[1]) == "-x") {
        int expected = 3; // 定义期望的参数数量
        if (argc != expected) {
            std::cerr << "Usage: " << argv[0] << " -x <gcx_file>" << std::endl;
            return 1;
        }
        Gcx gcx(argv[2]);
        gcx.open();
        ExtractStringData(gcx);
    } else if (std::string(argv[1]) == "-w") {
        int expected = 5; // 定义期望的参数数量
        if (argc != expected) {
            std::cerr << "Usage: " << argv[0] << " -w <txt_file> <base_offset> <gcx_file>" << std::endl;
            return 1;
        }
        const char* txtFile = argv[2];
        const char* baseOffset = argv[3];
        const char* gcxFile = argv[4];
        TXT txt(txtFile, baseOffset);
        Gcx gcx(gcxFile);
        gcx.open();
        txt.WriteStringToGcxdata(gcx);
        
        // 创建备份文件
        std::string backupFile = std::string(gcxFile) + ".bak";
        copyFile(gcxFile, backupFile);
        
        // 保存修改后的文件
        gcx.save(gcxFile);
    } else {
        std::cerr << "未知的模式: " << argv[1] << std::endl;
        return 1;
    }
    return 0;
}
