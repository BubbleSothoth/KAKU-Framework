#pragma once
#include "BufferFileInterface.h"
#include "IOContorller.h"

class OutBufferFileContorller :
	public BufferFileInterface, IOContorller {
public:
	OutBufferFileContorller(void) :
		BufferFileInterface(), IOContorller(IOContorller::IOSelection::out) {}
	OutBufferFileContorller(const wchar_t* file) :
		BufferFileInterface(file), IOContorller(IOContorller::IOSelection::out) {}
	~OutBufferFileContorller(void) = default;
};