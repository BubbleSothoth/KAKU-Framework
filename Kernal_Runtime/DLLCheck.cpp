#include "pch.h"
#include "DLLCheck.h"
#include "../Tools/HashAlgorithm.h"
#include <cstring>
#include <fstream>

DLLCheckManager::DLLCheckManager(std::string file, bool(*lpfunc)(const std::string, LPBYTE)) :
	chackFile(file), CheckFunc(lpfunc)
{
	__init__list();
	__substep__chack_process();
}

DLLCheckManager::~DLLCheckManager(void)
{
	for (auto it = this->DCIS_list.begin(); it != this->DCIS_list.end(); it++) {
		free(*it);
	}
	for (auto it = this->SampleList.begin(); it != this->SampleList.end(); it++) {
		free(*it);
	}
	this->DCIS_list.clear();
	this->SampleList.clear();
}

bool DLLCheckManager::GetAccess(void)
{
	return this->UnaccessList.empty();
}

void DLLCheckManager::__init__list(void)
{
	std::ifstream f(this->chackFile, std::ios_base::in | std::ios_base::binary);
	while (!f.eof()) {
		DCIS* dcis = (DCIS*)malloc(sizeof(DCIS));
		if (dcis == nullptr) { return; }
		memset(dcis, '\0', sizeof(DCIS));
		f.read((char*)dcis, sizeof(DCIS));
		if (strcpy_s(dcis->file, ""))
			this->DCIS_list.push_back(dcis);
		free(dcis);
	}
	f.close();
}


std::queue<struct DLLCheckReport*> DLLCheckManager::GetAccessList(void) {
	return this->AccessList;
}

std::queue<struct DLLCheckReport*> DLLCheckManager::GetUnaccessList(void) {
	return this->UnaccessList;
}

void DLLCheckManager::ShowMD5(BYTE* hash)
{
	__show__hash(hash, MD5Size);
	return;
}

void DLLCheckManager::ShowSHA256(BYTE* hash)
{
	__show__hash(hash, SHA256);
	return;
}

void DLLCheckManager::ShowCheckReport(DLLCheckReport* report)
{
	printf_s("文件名：%s\n", report->file);
	printf_s("样本值：");
	ShowMD5(report->MD5SimpleValue);
	printf_s("\n");
	printf_s("参考值：");
	ShowMD5(report->MD5StandardValue);
	printf_s("\n");
	printf_s("结论：%s\n", report->Access ? "通过" : "异常");
}

void DLLCheckManager::__substep__chack_process(void)
{
	if (this->CheckFunc == nullptr) { return; }
	for (int i=0;i< this->DCIS_list.size();i++) {
		DCIS* lpdcis = this->DCIS_list[i];

		DCIS* simple = (DCIS*)malloc(sizeof(DCIS));
		if (simple == nullptr) { return; }
		memset(simple, '\0', sizeof(DCIS));

		BYTE* hash = (BYTE*)malloc(MD5Size);
		if (hash == nullptr) { return; }
		memset(hash, '\0', MD5Size);

		strcpy_s(simple->file, 256, lpdcis->file);
		this->CheckFunc(lpdcis->file, hash);		
		memcpy_s(simple->md5Value, MD5Size, hash, MD5Size);

		free(hash);
		this->SampleList.push_back(simple);

		DCR* lpdcr = __create__DCR(simple->file, DCIS_Equal(simple, lpdcis));
		memcpy_s(lpdcr->MD5SimpleValue, MD5Size, simple->md5Value, MD5Size);
		memcpy_s(lpdcr->MD5StandardValue, MD5Size, lpdcis->md5Value, MD5Size);
		this->DLLCheckReport_list.push_back(lpdcr);

		free(simple);

		if (lpdcr->Access) {
			this->AccessList.push(lpdcr);
		}
		else {
			this->UnaccessList.push(lpdcr);
		}
	}
	return;
}

void DLLCheckManager::__show__hash(BYTE* hash, size_t size)
{
	for (int i = 0; i < size; i++) {
		printf_s("%02X ", *(hash + i));
	}
	return;
}

DLLCheckReport* DLLCheckManager::__create__DCR(std::string file, bool Access)
{
	DCR* dcr = (DCR*)malloc(sizeof(DCR));
	if (dcr == nullptr) {
		return nullptr;
	}
	memset(dcr->SHA256SimpleValue, '\0', SHA256Size);
	memset(dcr->SHA256StandardValue, '\0', SHA256Size);
	strcpy_s(dcr->file, 256, file.c_str());
	dcr->Access = Access;
	return dcr;
}

void DLLCheckManager::__erase__DCR(DLLCheckReport* lpdcr)
{
	if (lpdcr != nullptr) {
		free(lpdcr);
	}
	return;
}
