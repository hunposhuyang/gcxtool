#include <iostream>
#include "fileutil.h"
#include "gcx.h"
#include "string.h"

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
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "-x" && argc == 3) {
        const char* gcxFile = argv[2];

        Gcx gcx(gcxFile);
        gcx.open();
        ExtractStringData(gcx);

    } else if (mode == "-w" && argc == 5) {
        const char* txtFile = argv[2];
        const char* baseOffset = argv[3];
        const char* gcxFile = argv[4];

        TXT txt(txtFile, baseOffset);
        Gcx gcx(gcxFile);
        gcx.open();
        txt.WriteStringToGcxdata(gcx);
        gcx.open();
        gcx.save(gcxFile);
    } else {
        printUsage(argv[0]);
        return 1;
    }
    return 0;
}