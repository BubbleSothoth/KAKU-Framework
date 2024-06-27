#pragma once

#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <cstring>
#include <mutex>
#include <vector>
#include <Windows.h>
#include <wincrypt.h>

using namespace std;

#define SIZE (size_t)128

namespace Tools
{
	static void strSplit(char buf[SIZE][SIZE], const char* str, char ch = ' ') {
		int i, j, k;
		char temp[SIZE] = "";
		memset(temp, '\0', sizeof(temp));
		for (i = 0, j = 0, k = 0; str[i] != '\0'; i++) {
			if (str[i] == ch) {
				j = 0;
				strcpy_s(buf[k++], sizeof(char) * SIZE, temp);
				memset(temp, '\0', sizeof(temp));
				//printf("i=%d j=%d k=%d temp[%d]=%s\n",i,j,k,k-1,buf[k - 1]);
				continue;
			}
			else {
				temp[j++] = str[i];
			}
		}
		strcpy_s(buf[k], SIZE * sizeof(char), temp);
		//printf("i=%d j=%d k=%d temp[%d]=%s\n", i, j, k, k, buf[k]);
		return;
	}
	inline static void strSplit(char buf[SIZE][SIZE], std::string str, char ch = ' ') {
		strSplit(buf, str.c_str(), ch);
	}
	static void strSplit(std::string buf[SIZE], const std::string str, char ch = ' ', size_t buff_size = 128) {
        char pipe[SIZE][SIZE];
		memset(pipe, '\0', sizeof(pipe));
		strSplit(pipe, str, ch);
		for (int i = 0; pipe[i]!="" && i < buff_size; i++) {
			buf[i] = pipe[i];
			//printf("%s\n", buf[i]);
		}
		return;
	}
    /*
* 原始代码参考：https://blog.csdn.net/qq_30386941/article/details/126814596
* 开源协议：无
* 原注释：无
* 注释：wchar转换ANSI
*/
    static string WCharToAnsi(const wstring wstr) {
        string str;
        const wchar_t* lpstr = wstr.c_str();
        int nLen = WideCharToMultiByte(CP_ACP, 0, lpstr, -1, NULL, 0, NULL, NULL);
        if (nLen <= 0)
            return str;
        char* pszDst = (char*)malloc(nLen * sizeof(char));
        if (NULL == pszDst)
            return str;
        memset(pszDst, '\0', nLen * sizeof(char));
        WideCharToMultiByte(CP_ACP, 0, lpstr, -1, pszDst, nLen, NULL, NULL);
        str = string(pszDst);
        free(pszDst);
        return str;
    }

    /*
    * 原始代码参考：https://blog.csdn.net/qq_30386941/article/details/126814596
    * 开源协议：无
    * 原注释：无
    * 注释：ANSI转换wchar
    */
    static wstring AnsiToWChar(LPCSTR pszSrc, int nLen) {
        wstring wcharString;
        int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
        if (nSize <= 0)
            return wcharString;
        WCHAR* pwszDst = (wchar_t*)malloc((nSize + 1) * sizeof(wchar_t));
        if (NULL == pwszDst)
            return wcharString;
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
        *(pwszDst + nSize) = 0;
        if (*(pwszDst + 0) == 0xFEFF) // skip Oxfeff
            for (int i = 0; i < nSize; i++)
                *(pwszDst + i) = *(pwszDst + i + 1);
        wcharString = wstring(pwszDst);
        free(pwszDst);  // Report error
        return wcharString;
    }
    //重载:
    static wstring AnsiToWChar(string pszSrc) {
        return AnsiToWChar(pszSrc.c_str(), pszSrc.length());
    }

    /*
    * 原始代码参考：https://gitee.com/Euphony_Facetious/genshin-fps-unlock
    * 开源协议：无
    * 原注释：特征搜索 - 不是我写的 - 忘了在哪拷的
    * 注释：无
    */
    static uintptr_t PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for (auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for (auto j = 0ul; j < s; ++j) {
                if (scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return (uintptr_t)&scanBytes[i];
            }
        }
        return 0;
    }

    /*
    * 原始代码参考：https://gitee.com/Euphony_Facetious/genshin-fps-unlock
    * 开源协议：无
    * 原注释：无
    * 注释：获取字符串形式的错误信息
    */
    static std::string GetLastErrorAsString(DWORD code)
    {
        LPSTR buf = nullptr;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);
        std::string ret = buf;
        LocalFree(buf);
        return ret;
    }

};