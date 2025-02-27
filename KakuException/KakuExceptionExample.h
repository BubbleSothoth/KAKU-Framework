#pragma once
#include "KakuException.h"

class KakuFileNotOpenException :
    public KakuException
{
public:
    KakuFileNotOpenException(void) :
        KakuException(FILE_NOT_OPEN_ERROR, "KakuError: File not be opened."){}
    ~KakuFileNotOpenException(void) = default;
};

class KakuFileInputException :
    public KakuException {
public:
    KakuFileInputException(void) :
        KakuException(FILE_INPUT_ERROR, "KakuError: Can not input to the file.") {}
    ~KakuFileInputException(void) = default;
};

class KakuFileOutputException :
    public KakuException {
public:
    KakuFileOutputException(void) :
        KakuException(FILE_OUTPUT_ERROR, "KakuError: Can not output to the file.") {}
    ~KakuFileOutputException(void) = default;
};

class KakuMemoryException :
    public KakuException {
public:
    KakuMemoryException(void) :
        KakuException(MEMORY_ERROR, "KakuError: Memory Error, make sure your OS has enough memory to allocate.") {}
    ~KakuMemoryException(void) = default;
};

class KakuNullPointerException :
    public KakuException {
public:
    KakuNullPointerException(void) :
        KakuException(NULL_POINTER_ERROR, "KakuError: You used a pointer which point to the ZERO Adderss of memory.") {}
    ~KakuNullPointerException(void) = default;
};

class KakuDeadPointerException :
    public KakuException {
public:
    KakuDeadPointerException(void) :
        KakuException(DEAD_POINTER_ERROR, "KakuError: You used a pointer which point to the 0xDDDDDDDDDDDDDDDD Adderss of memory, which usually means that the memory be pointed by this pointer has been freed before.") {}
    ~KakuDeadPointerException(void) = default;
};

class KakuUnknownPointerException :
    public KakuException {
public:
    KakuUnknownPointerException(void) :
        KakuException(UNKNOWN_POINTER_ERROR, "KakuError: You used a pointer which point to the Unkonwn Adderss of memory, which usually means that the pointer you used without initialization and assignment.") {}
    ~KakuUnknownPointerException(void) = default;
};

class KakuNetworkConnectException :
    public KakuException {
public:
    KakuNetworkConnectException(void) :
        KakuException(CONNECT_ERROR, "KakuError: Can not connect to the Network.") {}
    ~KakuNetworkConnectException(void) = default;
};

class KakuUnresponseException :
    public KakuException {
public:
    KakuUnresponseException(void) :
        KakuException(UNRESPONSE_ERROR, "KakuError: Can not get response from the aim HOST.") {}
    ~KakuUnresponseException(void) = default;
};

class KakuSocketException :
    public KakuException {
public:
    KakuSocketException(void) :
        KakuException(_SOCKET_ERROR, "KakuError: Can not startup Socket Server.") {}
    ~KakuSocketException(void) = default;
};

class KakuBindException :
    public KakuException {
public:
    KakuBindException(void) :
        KakuException(BIND_ERROR, "KakuError: Can not bind to the aim address and port.") {}
    ~KakuBindException(void) = default;
};