#pragma once

#include "../DLLAPI.h"

#include "pch.h"
#include "../Tools/Score.h"

//RST表
#define RSTSIZE 0X100U
#define RSTCode unsigned short

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
#define ExitReadyFlag			0x12			//运行时系统Exit就绪（已运行RuntimeSystem::Exit方法完毕）
#define InputBufReadyFlag		0x13			//输入就绪标志，此标志为true说明输入缓冲区中准备好接收读入的数据，需要IO子系统进行读入
#define OutputBufReadyFlag		0x14			//输出就绪标志，此标志为true说明有需要输出的数据并已在输出缓冲区中准备好，需要IO子系统进行输出
#define InputFinishedFlag		0x15			//输入完成标志，此标志为true说明IO子系统已经完成读入操作，数据已经被记录在输入缓冲区中
#define OutputFinishedFlag		0x16			//输入完成标志，此标志为true说明IO子系统已经完成输出操作
// 可以根据需要添加自定义RST表项，值得注意的是默认RTS表最多支持255项（对应下标0x00-0xFF），超出此范围需要重新定义RSTSIZE宏。
// 建议顺序分配RTS表项的编号，避免表项过大导致的空间浪费，因为RST表在内存中是顺序存储的，也就是数组，RTS表项的编号实际上就是在
// 内存中根据基地址的偏移（下标），例如，编号为0x1000(4096)的表项意味着RST表至少有4097个元素。

#include "../KakuException/ErrorCode.h"

#ifndef _ErrorCode
#define _ErrorCode unsigned short
#endif // !_ErrorCode

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
#include "../KakuException/ErrorSystem.h"
#include "LogContorller.h"

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

class ErrorSystem;

inline void DLLAPI InitPrintf(std::string key, std::string value, unsigned short size = 20);

#pragma comment(lib,"../x64/Release/KakuException.lib")

template <class T>
struct IOData {
	char aim[10240];
	T data;
	size_t size;
	time_t timeStamp;
};

class DLLAPI RuntimeSystem
{
protected:
	LogContorller lc;
	PluginSystem pluginSys;
	static Score<bool[RSTSIZE]> runningStateTableScore;//RST表
	static Score<std::stack<_ErrorCode>> systemErrorTableScore;//SET表
	std::mutex ioMutex;		//IO线程锁，为了IO时的数据同步
	Score<struct IOData<void*>> inBuffer;
	Score<struct IOData<void*>> outBuffer;
	Score<struct CommandStruct> nextOperation;
	Score<std::stack<struct CommandStruct>> historyOperation;
	static BYTE exitCode;
	static wchar_t* inBufFile;
	static wchar_t* outBufFile;
private:
	BufferContorller bc;
	ErrorSystem* errorSys;
private:
	[[deprecated("已弃用该方法，请使用LOCK()宏代替")]]
	void __Lock(std::mutex& mtx) = delete;
	[[deprecated("已弃用该方法，请使用UNLOCK()宏代替")]]
	void __Unlock(std::mutex& mtx) = delete;
public:
	RuntimeSystem(void);
	RuntimeSystem(const RuntimeSystem&) = delete;	//禁止拷贝
	~RuntimeSystem(void);
	virtual void Init(void);
	virtual void Run(void);
	void Inte(void);
	virtual void Exit(void);
	void InputCommand(std::string) = delete;
	void _RecordError(std::string data, char* file, char* func, int line);
	void _RecordWarning(std::string data, char* file, char* func, int line);
	void _RecordInfo(std::string data, char* file, char* func, int line);
	void _RecordCust(std::string data, char* file, char* func, int line);
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
	void InputData(std::string aim, std::string& data);
	void InputData(std::string aim, BYTE* data, size_t data_size);
	void OutputData(std::string aim, std::string data);
	struct CommandStruct GetNextCommand(void);
private:
	void __boot(void);	//系统引导
	void __systemSelfInspection(void);
	void __systemSelfInspection_OS(void);
	void __systemSelfInspection_DLL(void);
	void __systemSelfInspection_FileIntegrity(void);
	void __systemRunningThreadeMode(void);
	void __init__config__input(void);
	void __Inte__substep__input__operation(void);
	void __Inte__substep__input(void);
	inline void __Inte__substep__output(void);
	template<class Subclass> void multithreadedMode(Subclass* lpsub);
	template<class Subclass> void singlethreadMode(Subclass* lpsub);
};

#define RecordInfo(mess) RuntimeSystem::_RecordInfo(mess,(char*)__FILE__,(char*)__FUNCTION__,__LINE__)
#define RecordError(mess) RuntimeSystem::_RecordError(mess,(char*)__FILE__,(char*)__FUNCTION__,__LINE__)
#define RecordWarning(mess) RuntimeSystem::_RecordWarning(mess,(char*)__FILE__,(char*)__FUNCTION__,__LINE__)
#define RecordCust(mess) RuntimeSystem::_RecordCust(mess,(char*)__FILE__,(char*)__FUNCTION__,__LINE__)

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

#define StartUp(Type) RuntimeSystem::MainProgress<Type>(this)
#include <map>
#define StepThread(step) std::pair<const char*, std::thread*>(step,nullptr)
#define th_Run thread_map["RUN"]
#define th_Inte thread_map["INTE"]
#define th_Plugin thread_map["PLUGIN"]

template<class Subclass>
void RuntimeSystem::multithreadedMode(Subclass* lpsub) {
	using std::thread;
	std::map<const char*, thread*> thread_map;// = { StepThread("RUN"), StepThread("INTE"), StepThread("INPUT"), StepThread("PLUGIN") };
	thread_map.insert(StepThread("RUN"));
	thread_map.insert(StepThread("INTE"));
	thread_map.insert(StepThread("INPUT"));
	thread_map.insert(StepThread("PLUGIN"));
	th_Inte = new thread(&RuntimeSystem::Inte, this);
	std::stringstream ss;
	//std::mutex ioMutex;
	th_Inte->detach();
	if (ioMutex.try_lock()) {
		ss << "Inte守护线程已启动,id:" << th_Inte->get_id();
		DB_INFO(ss.str().c_str());
		ioMutex.unlock();
	}
	lpsub->Init();
	while (!this->getRST(ExitFlag)) {
		th_Run= new std::thread(&Subclass::Run, lpsub);
		if (this->getRST(ThirdPartCodeAccessFlag)) th_Plugin = new std::thread(&RuntimeSystem::PluginTime, this);
		else th_Plugin = new std::thread();
		th_Run->join();
		if (this->getRST(ThirdPartCodeAccessFlag)) th_Plugin->join();
		delete th_Run;
		delete th_Plugin;
	}
	//先运行子类的Exit方法，再运行父类的Exit方法
	lpsub->Subclass::Exit();
	if (getRST(ExitReadyFlag)==false) lpsub->RuntimeSystem::Exit();

	delete th_Inte;
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