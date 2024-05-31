// CreateCheckFile.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../Tools/HashAlgorithm.h"
#include "../Kernal_Runtime/DLLCheckInfoStruct.h"

#pragma comment(lib,"X:/Reader/x64/Release/Tools.lib")

using namespace std;

const vector<string> file =
{
    "Kernal_Runtime.dll",
    "Kernal_PluginSystem.dll",
    "Kernal_ReaderSystem.dll",
    "DataStruct.dll",
    "Tools.dll",
    "Reader.exe"
};

bool __write(string file, DCIS* lpdata) {
    fstream fs(file, ios::app | ios::binary);
    if (!fs.is_open()) {
        return false;
    }
    fs.write((const char*)lpdata, sizeof(DCIS));
    fs.close();
    return true;
}

int main()
{
    fstream("check.md5", ios::out).close();
    LPBYTE hash = (LPBYTE)malloc(MD5Size);
    if (hash == nullptr) { return 0x1; }
    for (auto it = file.begin(); it != file.end(); it++) {
        DCIS dcis;
        memset(&dcis, 0, sizeof(DCIS));
        Tools::HashAlgorithm::MD5(*it, hash);
        strcpy_s(dcis.file, it->c_str());
        memcpy_s(dcis.md5Value, MD5Size, hash, MD5Size);
        printf_s("%s\t\t", it->c_str());
        for (int i = 0; i < MD5Size; i++) {
            printf_s("%02X ", *(hash + i));
        }
        printf_s("\n");
        __write("X:/Reader/check.md5", &dcis);
    }
    free(hash);
    return 0x0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
