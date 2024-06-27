#pragma once
#include "BufferFileInterface.h"

class InBufferFileContorller :
	public BufferFileInterface {
public:
	InBufferFileContorller(void) = default;
	InBufferFileContorller(const wchar_t* file) :
		BufferFileInterface(file) {
	}
	~InBufferFileContorller(void){}
	void Write(void* data, size_t size) = delete;
	void Append(void* data, size_t size) = delete;
};

