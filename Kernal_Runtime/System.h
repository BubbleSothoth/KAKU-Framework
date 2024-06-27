#pragma once

#include "../DLLAPI.h"

#include "pch.h"
#include "../Tools/Score.h"

//RST表长度
#define RSTSIZE 48

//RST表项
#define ExitFlag				0x00			//退出标识
#define ErrorFlag				0x01			//错误标识
#define FileOpenFlag			0x02			//文件开启标志
#define InputFlag				0x03			//文件正在写入标志
#define OutputFlag				0x04			//文件正在读取标志
#define IOFlag					0x05			//正在进行IO
#define ESCFlag					0x06			//紧急逃逸标志
#define FullConnectFlag			0x07			//连接满额标志
#define NoneConnectFlag			0x08			//网络无连接标志
#define InteFlag				0x09			//需要中断标志
#define ThreadModeDefineFlag	0x0A			//线程模式定义标志，若为false则需手动定义
#define ThreadMode				0x0B			//线程模式——true为多线程模式，false为单线程模式
#define BootFlag				0x0C			//引导标志——若Runtime已经加载则为true
#define SysMTXLockFlag			0x0D			//系统级线程锁锁定标志
#define PipeFlag				0x0E			//管道启动标志
#define ThirdPartCodeAccessFlag	0x0F			//接受第三方代码
#define RecodeLogFlag			0x10			//记录日志标志
#define BinFileCheckAccessFlag	0x11			//二进制文件校验通过标志
//ReaderSystem专用RST表项
#ifdef READER_SYSTEM
#define BookOpenFlag	0x10
#endif // READER_SYSTEM
//MinisterSystemSystem专用RST表项
#ifdef MINISTER_SYSTEM
#define TODO 0x20
#endif // MINISTER_SYSTEM

//ERROR
#define FILE_NOT_OPEN_ERROR		0x00
#define FILE_INPUT_ERROR		0x01
#define FILE_OUTPUT_ERROR		0x02
#define MEMORY_ERROR			0x10
#define NULL_POINTER_ERROR		0x20
#define DEAD_POINTER_ERROR		0x21
#define UNKNOW_POINTER_ERROR	0x22
#define NONE_BOOK_OPEN_ERROR	0x30
#define CONNECT_ERROR			0x40
#define UNRESPONSE_ERROR		0x41
#define _SOCKET_ERROR			0x42
#define BIND_ERROR				0x43

#define _ErrorCode unsigned short
#define RSTCode unsigned short

#include <stack>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <iostream>
#include <cstdarg>
#include <sstream>
#include <queue>
#include "../debug.h"
#include "../Tools/Score.h"
#include "BufferContorller.h"
#include "../Kernal_PluginSystem/PluginSystem.h"
#include "DLLCheck.h"

#include <time.h>
#include <sstream>

constexpr const char* NextOperationEmptyFlag = "###NULL###";

class CommandStruct {
public:
	time_t TimeStamp = 0;
	std::vector<std::string> Data = std::vector<std::string>(5, NextOperationEmptyFlag);
	CommandStruct(void) = default;
	CommandStruct(std::stringstream ss)
	{ 
		ss >> TimeStamp;
		for (int i = 0; i < 5; i++) {
			ss >> Data[i];
		}
	}
	~CommandStruct(void) = default;
	void Refresh(void) { *this = struct CommandStruct(); }
	bool IsEmpty(void) { return *this == struct CommandStruct(); }
	bool operator==(const struct CommandStruct& val) const { return this->TimeStamp == val.TimeStamp && this->Data == val.Data; }
	//bool operator==(const std::string& val) const { return this->Data == val; }
	bool operator!=(const struct CommandStruct& val) const { return !(val == *this); }
};
#define RST runningStateTableScore.GetValue()
#define SET systemErrorTableScore.GetValue()
#define RSTMutex runningStateTableScore.GetMutex()
#define SETMutex systemErrorTableScore.GetMutex()

inline void DLLAPI InitPrintf(std::string key, std::string value, unsigned short size = 20);

class DLLAPI RuntimeSystem
{
protected:
	BufferContorller bc;
	PluginSystem pluginSys;
	static Score<bool[RSTSIZE]> runningStateTableScore;//RST表
	//static bool runningStateTable[RSTSIZE];	
	//static std::stack<_ErrorCode> systemErrorTable;	
	static Score<std::stack<_ErrorCode>> systemErrorTableScore;//SET表
	std::mutex ioMutex;		//IO线程锁，为了IO时的数据同步
	Score<struct CommandStruct> nextOperation;
	Score<std::stack<struct CommandStruct>> historyOperation;
private:
	[[deprecated("已弃用该方法，请使用LOCK()宏代替")]]
	void __Lock(std::mutex& mtx);
	[[deprecated("已弃用该方法，请使用LOCK()宏代替")]]
	void __Unlock(std::mutex& mtx);
public:
	RuntimeSystem(void);
	RuntimeSystem(const RuntimeSystem&) = delete;	//禁止拷贝
	~RuntimeSystem(void);
	virtual void Init(void);
	virtual void Run(void);
	void Inte(void);
	virtual void Exit(void);
	void InputCommand(std::string) = delete;
	void RecordLog(void);
	void RcodeInfo(std::string data);
protected:
	void PluginTime(void);
	[[noinline]] static bool getRST(RSTCode);
	[[noinline]] static void setRST(RSTCode);
	[[noinline]] static void resetSET(void);
	[[noinline]] static void setSET(_ErrorCode);
	[[noinline]] static void resetRST(RSTCode);
	[[noinline]] static void showError(void);
	template<class Subclass> [[noinline]] inline void MainProgress(Subclass* lpsub);
	[[noinline]] static void setNormalError(_ErrorCode code);	//此方法用于设置一般性的错误
	[[noinline]] static void setSeriousError(_ErrorCode code);	//此方法设置的错误会令系统强制逃逸
	void OutputData(std::string aim, std::string data);
private:
	void __boot(void);	//系统引导
	void __systemSelfInspection(void);
	void __systemSelfInspection_OS(void);
	void __systemSelfInspection_DLL(void);
	void __systemSelfInspection_FileIntegrity(void);
	void __systemRunningThreadeMode(void);
	void __init__config__input(void);
	void __Inte__substep__input(void);
	inline void __Inte__substep__output(std::string aim, std::string data);
	template<class Subclass> void multithreadedMode(Subclass* lpsub);
	template<class Subclass> void singlethreadMode(Subclass* lpsub);
};

//Code
#define MultithreadedMode true
#define SinglethreadMode false

#define LOCK() ioMutex.lock();
#define UNLOCK() ioMutex.unlock();
#define AUTOLOCK() std::unique_lock<std::mutex> atlck(ioMutex);

template<class Subclass>
inline void RuntimeSystem::MainProgress(Subclass* lpsub) {
	getRST(ThreadMode) == MultithreadedMode ?
		this->multithreadedMode<Subclass>(lpsub) : this->singlethreadMode<Subclass>(lpsub);
	return;
}

template<class Subclass>
void RuntimeSystem::multithreadedMode(Subclass* lpsub) {
	std::thread th_Run;
	std::thread th_Inte;
	std::thread th_Input;
	std::thread th_Plugin;
	th_Inte = std::thread(&RuntimeSystem::Inte, this);
	std::stringstream ss;
	RuntimeSystem::ioMutex.lock();
	ss << "Inte守护线程已启动,id:" << th_Inte.get_id();
	DB_INFO(ss.str().c_str());
	RuntimeSystem::ioMutex.unlock();
	th_Inte.detach();
	lpsub->Init();
	while (!this->getRST(ExitFlag)) {
		th_Run = std::thread(&Subclass::Run, lpsub);
		if (this->getRST(ThirdPartCodeAccessFlag)) th_Plugin = std::thread(&RuntimeSystem::PluginTime,this);
		th_Run.join();
		if (this->getRST(ThirdPartCodeAccessFlag)) th_Plugin.join();
	}
	lpsub->Exit();
}

template<class Subclass>
void RuntimeSystem::singlethreadMode(Subclass* lpsub)
{
	lpsub->Subclass::Init();
	lpsub->Subclass::Inte();
	while (!getRST(ExitFlag))
	{
		lpsub->Subclass::Run();
		lpsub->Subclass::Inte();
		PluginTime();
	}
	lpsub->Subclass::Exit();
	return;
}