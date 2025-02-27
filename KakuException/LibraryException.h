#pragma once
#include <exception>
#include "ErrorCode.h"
class LibraryException :
    public std::exception
{
public:
    const _ErrorCode code = LIB_ERROR;
public:
    LibraryException(DATA_T mess) :
        std::exception(mess) {}
    ~LibraryException(void) = default;
    bool IsKakuException(void) {
        return !IsNotException() ? code != LIB_ERROR : false;
    }
    bool IsLibraryException(void) {
        return !IsNotException() ? code == LIB_ERROR : false;
    }
    bool IsNotException(void) {
        return code == NO_ERROR;
    }
};

