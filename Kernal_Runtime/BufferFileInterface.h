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
		unsigned int time = 1000000);
	~BufferFileInterface(void);
	bool Read(void* buf, size_t size);
	bool Write(void* data, size_t size);
	bool Append(void* data, size_t size);
private:
	HANDLE __open__file(int desired_access, char disposition);
};
