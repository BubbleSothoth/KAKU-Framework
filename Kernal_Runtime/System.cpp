#include "pch.h"
#include "System.h"

#include <iostream>
#include <thread>
#include <cstdarg>
#include <queue>
#include <Windows.h>

#include "SystemErrorPromationFactory.h"
#include "F:/project/include/debug.h"
#include "X:/Tools/Tools.h"
#include "BufferContorller.h"
#include "../third-party/mINI-master/src/mini/ini.h"

#pragma comment(lib,"X:/Reader/x64/Release/Tools.lib")
#pragma comment(lib, "X:/Reader/x64/Release/Kernal_PluginSystem.lib")

class __declspec(dllimport) PluginSystem;

//static declaration

Score<bool[RSTSIZE]> RuntimeSystem::runningStateTableScore;//RST表
Score<std::stack<_ErrorCode>> RuntimeSystem::systemErrorTableScore;//SET表
//code

inline void InitPrintf(std::string key, std::string value, unsigned short size) {
	std::string out;
	short len = size - key.length();
	len = len > 0 ? len : 0;
	out = key + std::string(len, '-') + value;
	printf_s("%s\n", out.c_str());
	return;
}

void RuntimeSystem::__Lock(std::mutex& mtx)
{
	mtx.lock();
}

void RuntimeSystem::__Unlock(std::mutex& mtx)
{
	mtx.unlock();
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
	return;
}

void RuntimeSystem::Run(void) {
}

void RuntimeSystem::Inte(void)
{
	SystemErrorPromationFactory fct;
	do {
		lock(RSTMutex, SETMutex);
		if (RST[ExitFlag]) {
			break;
		}
		while (this->RST[ErrorFlag] && !this->SET.empty()) {
			fct.create_new(this->SET.top());
			this->SET.pop();
		}
		SETMutex.unlock();
		RST[ErrorFlag] = false;
		if (this->RST[ESCFlag]) {
			exit(-1);
		}
		RSTMutex.unlock();
		__Inte__substep__input();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} while (getRST(ThreadMode));
	return;
}

void RuntimeSystem::Exit(void)
{
	bc.LogoutInFile();
	bc.LogoutOutFile();
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
	while (true) { std::this_thread::sleep_for(std::chrono::milliseconds(500)); }	//等待中断线程前来处理
}

inline void RuntimeSystem::OutputData(std::string aim, std::string data) {
	setRST(OutputFlag);
	__Inte__substep__output(aim, data);
	resetRST(OutputFlag);
}

void RuntimeSystem::__boot(void)
{
	__systemSelfInspection();
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

	value = ini.get("Plugin").get("ThirdPartCode");
	value == "0" ? resetRST(ThirdPartCodeAccessFlag) : setRST(ThirdPartCodeAccessFlag);
	value = ini.get("Plugin").get("DLLPath");
	if (value.back() != '\\')
		value += '\\';
	this->pluginSys.setsrcPath(value);
	return;
}

void RuntimeSystem::__Inte__substep__input(void) {
	char cmd[1024] = { '\0' };
	CommandStruct cs;
	if (!nextOperation.GetValue().IsEmpty()) {
		return;
	}
	bc.RegisterInFile(IN_BUFFERL);
	setRST(InputFlag);
	setRST(IOFlag);
	setRST(FileOpenFlag);
	bc.InFile->Read(cmd, 1024);
	resetRST(InputFlag);
	resetRST(IOFlag);
	resetRST(FileOpenFlag);
	bc.LogoutInFile();
	//printf_s("%s\n", cmd);
	cs = CommandStruct(std::stringstream(cmd));
	lock(historyOperation.GetMutex(), nextOperation.GetMutex());
	if (historyOperation.GetValue().empty() || historyOperation.GetValue().top() != cs) {
		nextOperation.GetValue() = cs;
	}
	historyOperation.GetMutex().unlock();
	nextOperation.GetMutex().unlock();
	return;
}

void RuntimeSystem::__Inte__substep__output(std::string aim, std::string data) {
	std::unique_lock<std::mutex> lock(this->ioMutex);
	setRST(OutputFlag);
	setRST(IOFlag);
	if (aim == "std") {
		std::cout << data << std::endl;
	}
	else {
		setRST(FileOpenFlag);
		wchar_t* temp = (wchar_t*)malloc(10240);
		if (temp == nullptr) {
			return;
		}
		lstrcpyW(temp, Tools::AnsiToWChar(aim).c_str());
		this->bc.RegisterOutFile(temp);
		bc.OutFile->Write((char*)data.c_str(), data.size());
		bc.LogoutOutFile();
		resetRST(FileOpenFlag);
		free(temp);
	}
	resetRST(OutputFlag);
	resetRST(IOFlag);
	return;
}