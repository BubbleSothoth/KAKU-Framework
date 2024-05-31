#include "pch.h"
#include "HashAlgorithm.h"
#include "Tools.h"

#include "../Reader/DLLAPI.h"

#include <iostream>
#include <wincrypt.h>
#include <Windows.h>

/*
* 原始代码参考：无
* 开源协议：无
* 原注释：无
* 注释：由ChatGPT-3.5编写的计算文件MD5值的算法
*/
bool Tools::HashAlgorithm::MD5(const char* filePath, LPBYTE hashBuffer) {
    // 打开文件
    std::cout << filePath << std::endl;
    HANDLE hFile = 0x0;
    hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] " << std::string(__FILE__) << ":" << __LINE__ << "\t" << "Failed to open file:" << std::string(filePath) << std::endl;
        return false;
    }

    // 创建哈希算法句柄
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) || !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        std::cerr << "Failed to create hash." << std::endl;
        CloseHandle(hFile);
        return false;
    }

    // 计算哈希值
    LPBYTE buffer;
    buffer = (LPBYTE)malloc(1024 * sizeof(BYTE));
    if (buffer == nullptr) {
        std::cerr << "Memory error." << std::endl;
        return false;
    }
    memset(buffer, 0, 1024 * sizeof(BYTE));
    DWORD bytesRead;
    while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
        if (!CryptHashData(hHash, buffer, bytesRead, 0)) {
            std::cerr << "Failed to hash data." << std::endl;
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            CloseHandle(hFile);
            return false;
        }
    }
    free(buffer);

    // 获取哈希值
    DWORD hashSize = 16;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hashBuffer, &hashSize, 0)) {
        std::cerr << "Failed to get hash value." << std::endl;
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);
        return false;
    }

    // 清理资源
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return true;
}

bool Tools::HashAlgorithm::MD5(const std::string filePath, LPBYTE hashBuffer) {
    return Tools::HashAlgorithm::MD5(filePath.c_str(), hashBuffer);
}