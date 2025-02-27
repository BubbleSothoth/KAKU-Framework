#pragma once
#include "BufferFileInterface.h"
#include "IOContorller.h"

class BufferFileContorller :
    public BufferFileInterface, IOContorller {
public:
    BufferFileContorller(void) :
        BufferFileInterface(), IOContorller(IOContorller::IOSelection::both) {}
    BufferFileContorller(IOContorller::IOSelection io_mode, const wchar_t* file) :
        BufferFileInterface(file), IOContorller(io_mode) {}
    ~BufferFileContorller(void) = default;
    bool Read(void* buf, size_t size) {
        if (this->IOContorller::InAccess()) {
            return BufferFileInterface::read(buf, size);
        }
        return false;
    }
    bool Write(void* data, size_t size) {
        if (this->IOContorller::OutAccess()) {
            return BufferFileInterface::write(data, size);
        }
        return false;
    }
    bool Append(void* data, size_t size) {
        if (this->IOContorller::OutAccess()) {
            return BufferFileInterface::append(data, size);
        }
        return false;
    }
};