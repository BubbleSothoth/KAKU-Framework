#pragma once
#include "../DLLAPI.h"
#include <stdexcept> 
#include <vector>
#include <mutex>
#include "KakuException.h"
#include "ErrorCode.h"

class DLLAPI ErrorSystem
{
private:
	static std::vector<KakuException> ErrorList;
	std::mutex mtx;
public:
	ErrorSystem(void);
	~ErrorSystem(void) = default;
	const KakuException GetErrorObject(_ErrorCode code);
};

