#pragma once
#include "BufferFileInterface.h"
#include "IOContorller.h"



class InBufferFileContorller :
	public BufferFileInterface,IOContorller {
public:
	InBufferFileContorller(void) :
		BufferFileInterface(),IOContorller(IOContorller::IOSelection::in){}
	InBufferFileContorller(const wchar_t* file) :
		BufferFileInterface(file), IOContorller(IOContorller::IOSelection::in) {}
	~InBufferFileContorller(void) = default;
};

