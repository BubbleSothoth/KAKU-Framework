#pragma once
#include "BufferFileInterface.h"

class OutBufferFileContorller :
	public BufferFileInterface {
public:
	OutBufferFileContorller(void) = default;
	OutBufferFileContorller(const wchar_t* file) :
		BufferFileInterface(file) {}
	~OutBufferFileContorller(void){}
	void Read(void* data, size_t size) = delete;
};