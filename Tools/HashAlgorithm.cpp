#include "pch.h"
#include "HashAlgorithm.h"
#include "Tools.h"

#include "../Reader/DLLAPI.h"

#include <iostream>
#include <wincrypt.h>
#include <Windows.h>

/*
* ԭʼ����ο�����
* ��ԴЭ�飺��
* ԭע�ͣ���
* ע�ͣ���ChatGPT-3.5��д�ļ����ļ�MD5ֵ���㷨
*/
bool Tools::HashAlgorithm::MD5(const char* filePath, LPBYTE hashBuffer) {
    // ���ļ�
    std::cout << filePath << std::endl;
    HANDLE hFile = 0x0;
    hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] " << std::string(__FILE__) << ":" << __LINE__ << "\t" << "Failed to open file:" << std::string(filePath) << std::endl;
        return false;
    }

    // ������ϣ�㷨���
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) || !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        std::cerr << "Failed to create hash." << std::endl;
        CloseHandle(hFile);
        return false;
    }

    // �����ϣֵ
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

    // ��ȡ��ϣֵ
    DWORD hashSize = 16;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hashBuffer, &hashSize, 0)) {
        std::cerr << "Failed to get hash value." << std::endl;
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);
        return false;
    }

    // ������Դ
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);

    return true;
}

bool Tools::HashAlgorithm::MD5(const std::string filePath, LPBYTE hashBuffer) {
    return Tools::HashAlgorithm::MD5(filePath.c_str(), hashBuffer);
}