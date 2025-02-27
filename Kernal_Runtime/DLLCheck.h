#pragma once
#include "DLLCheckInfoStruct.h"
#include <string>
#include <queue>
#include "../Tools/HashAlgorithm.h"

typedef struct DLLCheckReport {
	char file[256];
	bool Access;
	union {
		BYTE MD5SimpleValue[MD5Size];
		BYTE SHA256SimpleValue[SHA256Size];
	};
	union {
		BYTE MD5StandardValue[MD5Size];
		BYTE SHA256StandardValue[SHA256Size];
	};
}DLLCR, DCR;

class DLLCheckManager
{
private:
	std::string chackFile;
	std::vector<DCIS*> DCIS_list;
	std::vector<DCIS*>SampleList;
	std::vector<struct DLLCheckReport*> DLLCheckReport_list;
	std::queue<struct DLLCheckReport*> UnaccessList;
	std::queue<struct DLLCheckReport*> AccessList;
	bool (*CheckFunc)(const std::string, LPBYTE);
public:
	DLLCheckManager(std::string file, bool(*lpfunc)(const std::string, LPBYTE));
	~DLLCheckManager(void);
	bool GetAccess(void);
	std::queue<struct DLLCheckReport*> GetAccessList(void);
	std::queue<struct DLLCheckReport*> GetUnaccessList(void);
	static void ShowMD5(BYTE*);
	static void ShowSHA256(BYTE*);
	static void ShowCheckReport(struct DLLCheckReport*);
private:
	void __init__list(void);
	void __substep__chack_process(void);
	static void __show__hash(BYTE* hash, size_t size);
	struct DLLCheckReport* __create__DCR(std::string file, bool Access);
	void __erase__DCR(struct DLLCheckReport* lpdcr);
};

