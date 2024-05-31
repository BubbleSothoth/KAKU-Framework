#pragma once

#include "../Reader/DLLAPI.h"

#include <string>
#include <Windows.h>
namespace Tools
{
	namespace HashAlgorithm {
		bool DLLAPI MD5(const char* filePath, LPBYTE hashBuffer);
		bool DLLAPI MD5(const std::string filePath, LPBYTE hashBuffer);
		static void SHA256(void* buffer, std::string path);
	}
}

