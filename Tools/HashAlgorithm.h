#pragma once

#include "../DLLAPI.h"

#ifndef MD5Size

#define MD5Size (unsigned)128/8

#endif // !MD5Size

#ifndef SHA256Size

#define SHA256Size (unsigned)256/8

#endif // !MD5Size


#include <string>
#include <Windows.h>
namespace Tools
{
	namespace HashAlgorithm {
		bool DLLAPI MD5(const char* filePath, LPBYTE hashBuffer);
		bool DLLAPI MD5(const std::string filePath, LPBYTE hashBuffer);
		bool DLLAPI MD5(const BYTE * byteString, size_t byteSize, LPBYTE hashBuffer);
		bool DLLAPI Test(void);
		void DLLAPI SHA256(void* buffer, std::string path);
		bool DLLAPI MD5Equal(const BYTE md5_1[MD5Size], const BYTE md5_2[MD5Size]);
		bool DLLAPI SHA256Equal(const BYTE md5_1[MD5Size], const BYTE md5_2[MD5Size]);
	}
}
