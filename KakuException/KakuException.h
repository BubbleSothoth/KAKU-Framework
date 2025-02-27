#pragma once
#include <exception>
#include "ErrorCode.h"
#include"pch.h"

class KakuException :
    public std::exception
{
public:
    _ErrorCode code;
public:
    KakuException(void) { this->KakuException::KakuException(NONE_ERROR_OBJECT, "KakuError: Make sure you provided the right Error Code."); }
    KakuException(_ErrorCode err_code, DATA_T mess):
        std::exception(mess),code(err_code) {}
    ~KakuException(void) = default;
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