#pragma once

#define MD5Size (unsigned)128/8
#define SHA256Size (unsigned)256/8

typedef enum CheckAlgorithm
{
	MD5,
	SHA256
}CA;

typedef struct DLLCheckInfoStruct
{
	char file[256] = { '\0' };
	union
	{
		char md5Value[MD5Size];
		char sha256Value[SHA256Size];
	};
}DLL_CIS, DCIS;

bool DCIS_Equal(const DCIS* val1, const DCIS* val2);