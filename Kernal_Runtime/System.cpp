#include "pch.h"
#include "System.h"

#include <iostream>
#include <thread>
#include <cstdarg>
#include <queue>
#include <Windows.h>

#include "../debug.h"
#include "../Tools/Tools.h"
#include "BufferContorller.h"
#include "../third-party/mINI-master/src/mini/ini.h"

#pragma comment(lib,"../x64/Debug/Tools.lib")
#pragma comment(lib, "../x64/Debug/Kernal_PluginSystem.lib")

class __declspec(dllimport) PluginSystem;

//static declaration
BYTE RuntimeSystem::exitCode;
//ErrorSystem RuntimeSystem::errorSys;
Score<bool[RSTSIZE]> RuntimeSystem::runningStateTableScore;//RST表
Score<std::stack<_ErrorCode>> RuntimeSystem::systemErrorTableScore;//SET表
wchar_t* RuntimeSystem::inBufFile;
wchar_t* RuntimeSystem::outBufFile;

//code

inline void InitPrintf(std::string key, std::string value, unsigned short size) {
	std::string out;
	short len = size - key.length();
	len = len > 0 ? len : 0;
	out = key + std::string(len, '-') + value;
	printf_s("%s\n", out.c_str());
	return;
}

RuntimeSystem::RuntimeSystem(void)
{
	nextOperation.GetValue().Refresh();
	Init();
	return;
}

RuntimeSystem::~RuntimeSystem(void)
{
	lock(this->runningStateTableScore.GetMutex(), this->systemErrorTableScore.GetMutex());
	memset(this->runningStateTableScore.GetValue(), 0, sizeof(this->runningStateTableScore.GetValue()));
	while (!this->systemErrorTableScore.GetValue().empty()) {
		this->systemErrorTableScore.GetValue().pop();
	}
	this->runningStateTableScore.GetMutex().unlock();
	this->systemErrorTableScore.GetMutex().unlock();
}

void RuntimeSystem::Init(void)
{
	using std::cin;
	using std::cout;
	using std::endl;
	using std::string;
	using std::thread;
	string str;
	std::stringstream ss;
	SYSTEMTIME tsys;
	std::unique_lock<std::mutex> atlck(runningStateTableScore.GetMutex());
	GetLocalTime(&tsys);
	if (runningStateTableScore.GetValue()[BootFlag]) {
		return;
	}
	RuntimeSystem::exitCode = 0x00;
	cout << "-----------Boot System-----------" << endl;
	//this->pipe = *new Pipe();
	//OutPipeMoudle::registerPipeClient("KAKU_RuntimeSystem", Device::SystemLevel);
	//std::cout << "IOPipe-----------Access" << std::endl;
	InitPrintf("IOPipe", "Access");
	memset(runningStateTableScore.GetValue(), 0, sizeof(runningStateTableScore.GetValue()));
	//std::cout << "InitCheck--------Successful" << std::endl;
	InitPrintf("InitCheck", "Successful");
	atlck.unlock();
	__boot();
	InitPrintf("LoadingConfig", "Success");
	atlck.lock();
	this->pluginSys.InstallPlugin();
	InitPrintf("PluginSys", "Success");
	runningStateTableScore.GetValue()[BootFlag] = true;
	//ss << "[Log]" << tsys.wYear << "/" << tsys.wMonth << "/" << tsys.wDay << " " << tsys.wHour << ":" << tsys.wMinute << "\t" << "Runtime System Startup." << std::endl;
	cout << ss.str();
	this->errorSys = new ErrorSystem();
	this->historyOperation.GetMutex().lock();
	this->historyOperation.GetValue().push(CommandStruct(std::stringstream("0 start")));
	this->historyOperation.GetMutex().unlock();
	return;
}

void RuntimeSystem::Run(void) {
}

void RuntimeSystem::Inte(void)
{
	//SystemErrorPromationFactory fct;
	do {
		//printf_s("INTE\n");
		lock(RSTMutex, SETMutex);
		if (RST[ExitFlag]) {
			break;
		}
		while (this->RST[ErrorFlag] && !this->SET.empty()) {
			printf("%s\n",errorSys->GetErrorObject(this->SET.top()).what());
			//fct.create_new(this->SET.top());
			this->SET.pop();
		}
		SETMutex.unlock();
		RST[ErrorFlag] = false;
		if (this->RST[ESCFlag]) {
			RSTMutex.unlock();
			exit(RuntimeSystem::exitCode);
		}
		RSTMutex.unlock();
		__Inte__substep__input__operation();
		__Inte__substep__output();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (getRST(ThreadMode));
	return;
}

void RuntimeSystem::Exit(void)
{
	free(this->inBufFile);
	free(this->outBufFile);
	bc.LogoutInFile();
	bc.LogoutOutFile();
	delete this->errorSys;
	setRST(ExitReadyFlag);
	printf_s("Runtime System: Bye.\n");
}

void RuntimeSystem::_RecordError(std::string data, char* file, char* func, int line)
{
	LIP* log = (LIP*)malloc(sizeof(LIP));
	InitLogInfoStruct(log, "ERROR", data.c_str(), file, func, line);
	this->lc.Record(log);
	free(log);
}

void RuntimeSystem::_RecordWarning(std::string data, char* file, char* func, int line)
{
	LIP* log = (LIP*)malloc(sizeof(LIP));
	InitLogInfoStruct(log, "WARNING", data.c_str(), file, func, line);
	this->lc.Record(log);
	free(log);
}

void RuntimeSystem::_RecordInfo(std::string data, char* file, char* func, int line)
{
	LIP* log = (LIP*)malloc(sizeof(LIP));
	InitLogInfoStruct(log, "INFO", data.c_str(), file, func, line);
	this->lc.Record(log);
	free(log);
}

void RuntimeSystem::_RecordCust(std::string data, char* file, char* func, int line)
{
	LIP* log = (LIP*)malloc(sizeof(LIP));
	InitLogInfoStruct(log, "CUST", data.c_str(), file, func, line);
	this->lc.Record(log);
	free(log);
}

/**
void RuntimeSystem::InputCommand(std::string cmd)
{
	std::lock_guard<std::mutex> atlck(command_list_lock);
	std::string list[32];
	std::cout << "cmd:" << cmd << std::endl;
	Tools::strSplit(list, cmd, ' ', 32);
	return;
}
*/

void RuntimeSystem::PluginTime(void) {
	auto lp = pluginSys.AccessPlugin();
	if (lp!=nullptr){ lp->Call("Inlet"); }
}

[[noinline]] bool RuntimeSystem::getRST(RSTCode code)
{
	std::unique_lock<std::mutex> atlck(RSTMutex);
	bool rtn = RuntimeSystem::RST[code];
	return rtn;
}

[[noinline]] void RuntimeSystem::setRST(RSTCode code)
{
	std::unique_lock<std::mutex> atlck(RSTMutex);
	RuntimeSystem::RST[code] = true;
}

[[noinline]] void RuntimeSystem::resetRST(RSTCode code)
{
	std::unique_lock<std::mutex> atlck(RSTMutex);
	RuntimeSystem::RST[code] = false;
}

[[noinline]] void RuntimeSystem::setSET(_ErrorCode code)
{
	std::unique_lock<std::mutex> atlck(SETMutex);
	RuntimeSystem::SET.push(code);
	return;
}

[[noinline]] void RuntimeSystem::resetSET()
{
	std::unique_lock<std::mutex> atlck(SETMutex);
	while (!RuntimeSystem::SET.empty())
		RuntimeSystem::SET.pop();
	return;
}

[[noinline]] void RuntimeSystem::showError(void)
{
	std::unique_lock<std::mutex> atlck(SETMutex);
	_ErrorCode code = 0xff;
	while (!RuntimeSystem::SET.empty()) {
		code = RuntimeSystem::SET.top();
		RuntimeSystem::SET.pop();
		RuntimeSystem::setRST(OutputFlag);
		std::cout << code << std::endl;
		RuntimeSystem::resetRST(OutputFlag);
	}
}

[[noinline]] void RuntimeSystem::setNormalError(_ErrorCode code) {
	lock(SETMutex, RSTMutex);
	int sum = code;
	SET.push(code);
	RST[ErrorFlag] = true;
	RST[InteFlag] = true;
	SETMutex.unlock();
	RSTMutex.unlock();
}

void RuntimeSystem::setSeriousError(_ErrorCode code) {
	lock(SETMutex, RSTMutex);
	SET.push(code);
	RST[ErrorFlag] = true;
	RST[ESCFlag] = true;
	RST[InteFlag] = true;
	SETMutex.unlock();
	RSTMutex.unlock();
	RuntimeSystem::exitCode = ~code;
	while (true) { std::this_thread::sleep_for(std::chrono::milliseconds(500)); }	//等待中断线程前来处理
}

void RuntimeSystem::InputData(std::string aim, BYTE* data, size_t data_size)
{
	std::unique_lock<std::mutex> lock(this->inBuffer.GetMutex());
	while (getRST(InputBufReadyFlag)) {
		lock.unlock();
		std::this_thread::yield();
		lock.lock();
	}
	strcpy_s(this->inBuffer.GetValue().aim, aim.c_str());
	this->inBuffer.GetValue().size = data_size;
	this->inBuffer.GetValue().data = malloc(data_size * sizeof(BYTE));
	ZeroMemory(this->inBuffer.GetValue().data, this->inBuffer.GetValue().size);
	setRST(InputBufReadyFlag);

	do {
		lock.unlock();
		this_thread::yield();
		lock.lock();
	} while (!getRST(InputFinishedFlag));

	memcpy_s(data, data_size, this->inBuffer.GetValue().data, this->inBuffer.GetValue().size);
	ZeroMemory(this->inBuffer.GetValue().aim, sizeof(this->inBuffer.GetValue().aim));
	free(this->inBuffer.GetValue().data);
	resetRST(InputBufReadyFlag);
	resetRST(InputFinishedFlag);

	return;
}

inline void RuntimeSystem::OutputData(std::string aim, std::string data) {
	std::unique_lock<std::mutex> lock(this->outBuffer.GetMutex());
	while (getRST(OutputBufReadyFlag)) {
		std::this_thread::yield();
	}
	strcpy_s(this->outBuffer.GetValue().aim, aim.c_str());
	this->outBuffer.GetValue().size = aim.length() * sizeof(char) + 1;
	this->outBuffer.GetValue().data = malloc(this->outBuffer.GetValue().size);
	ZeroMemory(this->outBuffer.GetValue().data, this->outBuffer.GetValue().size);
	memcpy_s(this->outBuffer.GetValue().data, this->outBuffer.GetValue().size, data.c_str(), data.length() * sizeof(char));
	//__Inte__substep__output(aim, data);
	setRST(OutputBufReadyFlag);

	do {
		lock.unlock();
		this_thread::yield();
		lock.lock();
	} while (!getRST(OutputFinishedFlag));
	ZeroMemory(this->outBuffer.GetValue().aim, sizeof(this->outBuffer.GetValue().aim));
	free(this->outBuffer.GetValue().data);

	resetRST(OutputBufReadyFlag);
	resetRST(OutputFinishedFlag);
	
	return;
}

struct CommandStruct RuntimeSystem::GetNextCommand(void)
{
	struct CommandStruct rtn_cs;
	lock(this->nextOperation.GetMutex(), this->historyOperation.GetMutex());

	if (this->nextOperation.GetValue() == this->historyOperation.GetValue().top())
		rtn_cs = struct CommandStruct();
	else{
		rtn_cs = this->nextOperation.GetValue();
		this->historyOperation.GetValue().push(this->nextOperation.GetValue());
	}

	this->historyOperation.GetMutex().unlock();
	this->nextOperation.GetMutex().unlock();
	return rtn_cs;
}

void RuntimeSystem::__boot(void)
{
//	__systemSelfInspection();
	__init__config__input();
	__systemRunningThreadeMode();
}

void RuntimeSystem::__systemSelfInspection(void)
{
	if (false) {
		//std::cout << "ShareMemory------ABNORMAL\n";
		InitPrintf("ShareMemory", "ABNORMAL");
		this->setNormalError(MEMORY_ERROR);
	}
	else {
		//std::cout << "ShareMemory------NORMAL\n";
		InitPrintf("ShareMemory", "NORMAL");
	}
	__systemSelfInspection_OS();
	__systemSelfInspection_DLL();
	__systemSelfInspection_FileIntegrity();
	return;
}

void RuntimeSystem::__systemSelfInspection_OS(void)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	// 在这里设置条件掩码，指定你想要检查的操作系统版本
	// 这里以 Windows 10 为例
	osvi.dwMajorVersion = 10;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 0;
	osvi.wProductType = VER_NT_WORKSTATION;

	// 设置条件掩码
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);

	// 调用 VerifyVersionInfo 函数
	if (!VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE, dwlConditionMask)) {
		InitPrintf("OSCheck", "ACCESS");
	} 
	else {
		//std::cout<<"UNSUCCESS" << std::endl;
		InitPrintf("OSCheck", "UNSUCCESS");
		exit(-1);
	}
}

void RuntimeSystem::__systemSelfInspection_DLL(void)
{
	DLLCheckManager* checker = new DLLCheckManager("check.md5", &Tools::HashAlgorithm::MD5);
	if (checker == nullptr) {
		InitPrintf("DLLCheck", "UNCHECK");
		resetRST(BinFileCheckAccessFlag);
		return;
	}
	//std::cout << "DLLCheck---------";
	if (checker->GetAccess())	//TODO
	{
		InitPrintf("DLLCheck", "ACCESS");
		resetRST(BinFileCheckAccessFlag);
	}
	else {
		//std::cout << "Missing DLL File" << std::endl;
		InitPrintf("DLLCheck", "UNACCESS");
		auto list = checker->GetUnaccessList();
		printf_s("\n");
		while (!list.empty()) {
			DLLCheckManager::ShowCheckReport(list.front());
			list.pop();
		}
		setRST(BinFileCheckAccessFlag);
	}
	delete checker;
	return;
}

void RuntimeSystem::__systemSelfInspection_FileIntegrity(void)
{
	//std::cout << "FileCheck--------";
	while (true) //TODO
	{
		if (false) {
			//std::cout << "Unauthorized file" << std::endl;
			InitPrintf("FileCheck", "Unauthorized file");
			return;
		}
		else {
			break;
		}
	}
	InitPrintf("FileCheck", "ACCESS");
	//std::cout << "ACCESS" << std::endl;
}

void RuntimeSystem::__systemRunningThreadeMode(void)
{
	std::string str;
	if (this->RST[ThreadModeDefineFlag]) {
		return;
	}
	std::cout << "Thread mode (single or multy): ";
	std::cin >> str;
	if (str == "single" || str == "s") {
		this->RST[ThreadMode] = SinglethreadMode;
	}
	else if (str == "multy" || str == "m") {
		this->RST[ThreadMode] = MultithreadedMode;
	}
	else if (str == "shutdown") {
		this->RST[ExitFlag] = true;
	}
	else {
		str = "";
		__systemRunningThreadeMode();
	}
	scanf_s("%*c");
	return;
}

void RuntimeSystem::__init__config__input(void)
{
	mINI::INIFile file("KAKU.ini");
	mINI::INIStructure ini;
	file.read(ini);
	std::string value = ini.get("Init").get("ThreadMode");
	setRST(ThreadModeDefineFlag);
	if (value == "m")
		setRST(ThreadMode);
	else if (value == "s")
		setRST(ThreadMode);
	else
		resetRST(ThreadModeDefineFlag);

	value = ini.get("Init").get("RecodeLog");
	value == "0" ? resetRST(RecodeLogFlag) : setRST(RecodeLogFlag);
	value = ini.get("Init").get("LogFile");
	if (!value.empty()) this->lc.RegistLogFile(value.c_str());

	value = ini.get("Init").get("BufferPath");
	if (value != "") {
		std::wstring inwstr = Tools::AnsiToWChar(value) + L"in.buf";
		std::wstring outwstr = Tools::AnsiToWChar(value) + L"out.buf";
		std::wcout << std::wstring(inwstr) << L"\n" << std::wstring(outwstr) << L"\n";
		this->inBufFile = (wchar_t*)calloc(lstrlenW(inwstr.c_str()) + 1, sizeof(wchar_t)); 
		this->outBufFile = (wchar_t*)calloc(lstrlenW(outwstr.c_str()) + 1, sizeof(wchar_t));
		if (this->inBufFile == nullptr || this->outBufFile == nullptr) setSeriousError(MEMORY_ERROR);
		std::wcout << std::wstring(inwstr) << L"\n" << std::wstring(outwstr) << L"\n";
		lstrcpyW(this->inBufFile, inwstr.c_str());
		lstrcpyW(this->outBufFile, outwstr.c_str());
	}
	else {
		this->inBufFile = (wchar_t*)IN_BUFFERL;
		this->outBufFile = (wchar_t*)OUT_BUFFERL;
	}
	bc.RegisterInFile(this->inBufFile);
	bc.RegisterOutFile(this->outBufFile);

	value = ini.get("Plugin").get("ThirdPartCode");
	if (value == "1")
		setRST(ThirdPartCodeAccessFlag);
	else
		resetRST(ThirdPartCodeAccessFlag);
	value = ini.get("Plugin").get("DLLPath");
	if (!value.empty()) {
		if (value.back() != '\\')
			value += '\\';
		this->pluginSys.setsrcPath(value);
	}
	return;
}

void RuntimeSystem::__Inte__substep__input__operation(void) {
	char cmd[1024] = { '\0' };
	struct CommandStruct cs;
	bool is_new = false;

	lock(historyOperation.GetMutex(), nextOperation.GetMutex());
	is_new = nextOperation.GetValue() == historyOperation.GetValue().top();
	historyOperation.GetMutex().unlock();
	nextOperation.GetMutex().unlock();

	if (!is_new) 
		return;

	//bc.RegisterInFile(IN_BUFFERL);
	setRST(InputFlag);
	setRST(IOFlag);
	setRST(FileOpenFlag);
	bc.RegisterInFile(this->inBufFile);
	bc.InFile->Read(cmd, 1024);
	printf_s("read %s\n",cmd);
	resetRST(InputFlag);
	resetRST(IOFlag);
	resetRST(FileOpenFlag);
	//bc.LogoutInFile();
	//printf_s("%s\n", cmd);

	nextOperation.GetMutex().lock();
	nextOperation.GetValue() = std::stringstream(cmd);
	nextOperation.GetMutex().unlock();
	return;
}

void RuntimeSystem::__Inte__substep__input(void) {
	std::unique_lock<std::mutex> iolock(this->ioMutex), bfflock(this->outBuffer.GetMutex());
	std::string aim = this->inBuffer.GetValue().aim;
	void* data = this->inBuffer.GetValue().data;
	size_t size = this->inBuffer.GetValue().size;
	if (!getRST(InputBufReadyFlag)) return;
	setRST(InputFlag);
	setRST(IOFlag);

	if (aim == "std") {
		using namespace std;
		string _;
		cin >> _;
		strcpy_s((char*)data, size, _.c_str());
	}
	else if (aim.empty() || data == nullptr) {
		return;
	}
	else if (aim == "nul") {
		ZeroMemory(data, size);
	}
	else {
		setRST(FileOpenFlag);
		wchar_t* temp = (wchar_t*)malloc(sizeof(this->inBuffer.GetValue().aim));
		if (temp == nullptr) {
			return;
		}
		lstrcpyW(temp, Tools::AnsiToWChar(aim).c_str());
		this->bc.RegisterInFile(temp);
		bc.OutFile->Read(data, size);
		bc.LogoutOutFile();
		resetRST(FileOpenFlag);
		free(temp);
	}
	setRST(InputFinishedFlag);
	resetRST(InputFlag);
	resetRST(IOFlag);
}

void RuntimeSystem::__Inte__substep__output() {
	std::unique_lock<std::mutex> iolock(this->ioMutex), bfflock(this->outBuffer.GetMutex());
	std::string aim = this->outBuffer.GetValue().aim;
	void* data = this->outBuffer.GetValue().data;
	size_t size = this->outBuffer.GetValue().size;
	if (!getRST(OutputBufReadyFlag)) return;
	setRST(OutputFlag);
	setRST(IOFlag);
	if (aim == "std") {
		std::cout << std::string((const char*)this->outBuffer.GetValue().data) << std::endl;
	}
	else if (aim.empty() || data==nullptr) {
		return;
	}
	else if (aim == "nul") {
		ZeroMemory(data, size);
	}
	else {
		setRST(FileOpenFlag);
		wchar_t* temp = (wchar_t*)malloc(10240);
		if (temp == nullptr) {
			return;
		}
		lstrcpyW(temp, Tools::AnsiToWChar(aim).c_str());
		this->bc.RegisterOutFile(temp);
		bc.OutFile->Write((char*)this->outBuffer.GetValue().data, this->outBuffer.GetValue().size);
		bc.LogoutOutFile();
		resetRST(FileOpenFlag);
		free(temp);
	}
	//strcat_s(this->outBuffer.GetValue().aim, "-Finished");
	resetRST(OutputFlag);
	resetRST(IOFlag);

	free(this->outBuffer.GetValue().data);
	ZeroMemory(&this->outBuffer.GetValue(), sizeof(this->outBuffer.GetValue()));
	resetRST(OutputFinishedFlag);
	return;
}