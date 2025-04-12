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

void FileCrypt(bool mode, const char* gcxFilepath, const char* Key) {
    std::string input = gcxFilepath;
    std::string outputFile = gcxFilepath;
    resetDir(outputFile);

    if (!checkFileExists(gcxFilepath)) return;
    if (!checkKeySet(Key)) return;

    Crypto crypto(Key);

    if (mode) {
        std::string backupFile = input + "_noncrypt.bak";
        if (!copyFile(input, backupFile)) {
            printf("Failed to create backup file: %s\n", backupFile.c_str());
            return;
        }
        crypto.encrypt(input, outputFile);
    } else {
        std::string backupFile = input + "_encrypt.bak";
        if (!copyFile(input, backupFile)) {
            printf("Failed to create backup file: %s\n", backupFile.c_str());
            return;
        }
        crypto.decrypt(input, outputFile);
    }
}

void ExtractStringData(Gcx &gcx) {
    gcx.CreateOffsetStringMap();
    std::filesystem::path gcxPath(gcx.gcxfilepath);
    std::string output = gcxPath.parent_path().u8string() + "/" + getExtensionlessName(gcx.gcxfilepath) + "_" + getFileDirName(gcx.gcxfilepath) + ".txt";
    std::ofstream offsetFile(output);
    if (!offsetFile.is_open()) {
        std::cerr << "Failed to create offset file" << std::endl;
        return;
    }

    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    offsetFile.write(reinterpret_cast<char*>(bom), sizeof(bom));

    offsetFile << std::hex << std::setfill('0');

    for (const auto& table : gcx.stringTables) {
        offsetFile << "0x" << std::setw(8) << table.offset << "\t" << table.string << std::endl;
    }

    offsetFile.close();
    std::cout << output << " created successfully." << std::endl;
}

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [-x|-w] [options]..." << std::endl;
    std::cerr << "  -x <gcx_file>        Extract string data from GCX file" << std::endl;
    std::cerr << "  -w <txt_file> <base_offset> <gcx_file>  Write string data to GCX file" << std::endl;
    std::cerr << "  -k <key>             Key for encryption/decryption" << std::endl;
    std::cerr << "  -k <key> <gcx_file>  encryption/decryption" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    const char* key = nullptr;

    // 检查是否有 -k 参数
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-k" && i + 1 < argc) {
            key = argv[i + 1];
            break;
        }
    }

    if (mode == "-x" && (argc == 3 || (argc == 5 && key != nullptr))) {
        const char* gcxFile = argv[2];
        if (key != nullptr) {
            FileCrypt(false, gcxFile, key); // 解密文件
        }
        Gcx gcx(gcxFile);
        gcx.open();
        ExtractStringData(gcx);

    } else if (mode == "-w" && (argc == 5 || (argc == 7 && key != nullptr))) {
        const char* txtFile = argv[2];
        const char* baseOffset = argv[3];
        const char* gcxFile = argv[4];

        TXT txt(txtFile, baseOffset);
        Gcx gcx(gcxFile);
        gcx.open();
        txt.WriteStringToGcxdata(gcx);
        gcx.open();
        gcx.save(gcxFile);
        if (key != nullptr) {
            FileCrypt(true, gcxFile, key); // 加密文件
        }
    } else if (mode == "-k" && argc == 4 && key != nullptr)
    {
        const char* gcxFile = argv[3];
        FileCrypt(true, gcxFile, key); // 加密文件
    }else {
        printUsage(argv[0]);
        return 1;
    }
    return 0;
}