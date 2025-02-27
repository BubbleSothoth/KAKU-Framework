#pragma once

//#include "InBufferFileContorller.h"
//#include "OutBufferFileContorller.h"
#include "BufferFileContorller.h"

constexpr const wchar_t* IN_BUFFERL = L"C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\in.buf";
constexpr const wchar_t* OUT_BUFFERL = L"C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\out.buf";
constexpr const char* IN_BUFFER = "C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\in.buf";
constexpr const char* OUT_BUFFER = "C:\\Users\\27746\\AppData\\Local\\Temp\\KakuBuffer\\out.buf";

class BufferContorller
{
public:
	BufferFileContorller* InFile = nullptr;
	BufferFileContorller* OutFile = nullptr;
	BufferContorller() = default;
	~BufferContorller() = default;
public:
	void RegisterOutFile(const wchar_t* file) {
		LogoutOutFile();
		this->OutFile = new BufferFileContorller(IOContorller::IOSelection::out, file);
	}

	void RegisterInFile(const wchar_t* file) {
		LogoutInFile();
		this->InFile = new BufferFileContorller(IOContorller::IOSelection::in,  file);
		return;
	}

	inline void LogoutInFile(void){
		if (this->InFile != nullptr) {
			delete this->InFile;
			this->InFile = nullptr;
		}
	}

	inline void LogoutOutFile(void) {
		if (this->OutFile != nullptr) { 
			delete this->OutFile; 
			this->OutFile = nullptr;
		}
	}
};