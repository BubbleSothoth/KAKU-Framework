#pragma once

#include <fstream>
#include "../Tools/Tools.h"

class BufferFileInterface {
public:
	wchar_t* bufferFile = nullptr;
	unsigned int survivalTime = 60;
public:
	BufferFileInterface() = default;
	BufferFileInterface(
		const wchar_t* file,
		unsigned int time = 1000000):
		bufferFile(nullptr), survivalTime(time)
	{
		this->bufferFile = (wchar_t*)malloc((lstrlenW(file) + 1) * sizeof(wchar_t));
		if (this->bufferFile == nullptr) return;
		lstrcpyW(this->bufferFile, file);
		return;
	}
	~BufferFileInterface(void){
		free(this->bufferFile);
	}
protected:
	bool read(void* buf, size_t size) {
		DWORD dw;
		HANDLE hfile;
		hfile = __open__file(GENERIC_READ, OPEN_EXISTING);
		if (hfile == 0x0) {
			printf_s("0x0 error\n");
			return false;
		}
		if (!ReadFile(hfile, buf, size, &dw, nullptr)) {
			printf_s("Read error\n");
			return false;
		}
		CloseHandle(hfile);
		return true;
	}
	bool write(void* data, size_t size) {
		DWORD dw;
		HANDLE hfile;
		hfile = __open__file(GENERIC_WRITE, CREATE_ALWAYS);
		if (hfile == 0x0) {
			return false;
		}
		if (!WriteFile(hfile, (char*)data, size, &dw, nullptr)) {
			return false;
		}
		CloseHandle(hfile);
		return true;
	}
	bool append(void* data, size_t size) {
		DWORD dw;
		HANDLE hfile;
		hfile = __open__file(FILE_APPEND_DATA, OPEN_ALWAYS);
		if (hfile == 0x0) {
			return false;
		}
		if (!WriteFile(hfile, data, size, &dw, nullptr)) {
			return false;
		}
		CloseHandle(hfile);
		return true;
	}
private:
	HANDLE __open__file(int desired_access, char disposition) {
		HANDLE hfile;
		unsigned int time = 0;
		while (true) {
			hfile = CreateFile(this->bufferFile,
				desired_access,
				0x00000000,
				NULL,
				disposition,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (hfile != INVALID_HANDLE_VALUE) { break; }
			printf_s("发生文件碰撞\n");
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			if (++time == survivalTime) { break; }
		}
		if (time == survivalTime) { return 0x0; }
		return hfile;
	}
};
