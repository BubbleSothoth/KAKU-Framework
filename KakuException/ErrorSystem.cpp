#include "pch.h"
#include "ErrorSystem.h"
#include "KakuExceptionExample.h"

std::vector<KakuException> ErrorSystem::ErrorList;

ErrorSystem::ErrorSystem(void)
{
	std::unique_lock<std::mutex> atlck(mtx);
	ErrorList.resize((size_t)MaxErrorNumber);
	static const KakuException empty_excp = KakuException(NONE_ERROR_OBJECT, "KakuError: Make sure you provided the right Error Code.");
	for (auto it = ErrorList.begin(); it != ErrorList.end();it++) {
		*it = empty_excp;
	}
	
	ErrorList[FILE_NOT_OPEN_ERROR]	 =	KakuFileNotOpenException();
	ErrorList[FILE_INPUT_ERROR]		 =	KakuFileInputException();
	ErrorList[FILE_OUTPUT_ERROR]	 =	KakuFileOutputException();
	ErrorList[MEMORY_ERROR]			 =	KakuMemoryException();
	ErrorList[NULL_POINTER_ERROR]	 =	KakuNullPointerException();
	ErrorList[DEAD_POINTER_ERROR]	 =	KakuDeadPointerException();
	ErrorList[UNKNOWN_POINTER_ERROR] =	KakuUnknownPointerException();
	ErrorList[UNRESPONSE_ERROR]		 =	KakuUnresponseException();
	ErrorList[_SOCKET_ERROR]		 =	KakuSocketException();
	ErrorList[BIND_ERROR]			 =	KakuBindException();
}

const KakuException ErrorSystem::GetErrorObject(_ErrorCode code)
{
	std::unique_lock<std::mutex> atlck(mtx);
	return ErrorList[code];
}
