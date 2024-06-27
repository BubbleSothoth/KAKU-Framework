#pragma once

#include "../DLLAPI.h"

#include "pch.h"
#include "../Tools/Score.h"

//RST����
#define RSTSIZE 48

//RST����
#define ExitFlag				0x00			//�˳���ʶ
#define ErrorFlag				0x01			//�����ʶ
#define FileOpenFlag			0x02			//�ļ�������־
#define InputFlag				0x03			//�ļ�����д���־
#define OutputFlag				0x04			//�ļ����ڶ�ȡ��־
#define IOFlag					0x05			//���ڽ���IO
#define ESCFlag					0x06			//�������ݱ�־
#define FullConnectFlag			0x07			//���������־
#define NoneConnectFlag			0x08			//���������ӱ�־
#define InteFlag				0x09			//��Ҫ�жϱ�־
#define ThreadModeDefineFlag	0x0A			//�߳�ģʽ�����־����Ϊfalse�����ֶ�����
#define ThreadMode				0x0B			//�߳�ģʽ����trueΪ���߳�ģʽ��falseΪ���߳�ģʽ
#define BootFlag				0x0C			//������־������Runtime�Ѿ�������Ϊtrue
#define SysMTXLockFlag			0x0D			//ϵͳ���߳���������־
#define PipeFlag				0x0E			//�ܵ�������־
#define ThirdPartCodeAccessFlag	0x0F			//���ܵ���������
#define RecodeLogFlag			0x10			//��¼��־��־
#define BinFileCheckAccessFlag	0x11			//�������ļ�У��ͨ����־
//ReaderSystemר��RST����
#ifdef READER_SYSTEM
#define BookOpenFlag	0x10
#endif // READER_SYSTEM
//MinisterSystemSystemר��RST����
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
	static Score<bool[RSTSIZE]> runningStateTableScore;//RST��
	//static bool runningStateTable[RSTSIZE];	
	//static std::stack<_ErrorCode> systemErrorTable;	
	static Score<std::stack<_ErrorCode>> systemErrorTableScore;//SET��
	std::mutex ioMutex;		//IO�߳�����Ϊ��IOʱ������ͬ��
	Score<struct CommandStruct> nextOperation;
	Score<std::stack<struct CommandStruct>> historyOperation;
private:
	[[deprecated("�����ø÷�������ʹ��LOCK()�����")]]
	void __Lock(std::mutex& mtx);
	[[deprecated("�����ø÷�������ʹ��LOCK()�����")]]
	void __Unlock(std::mutex& mtx);
public:
	RuntimeSystem(void);
	RuntimeSystem(const RuntimeSystem&) = delete;	//��ֹ����
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
	[[noinline]] static void setNormalError(_ErrorCode code);	//�˷�����������һ���ԵĴ���
	[[noinline]] static void setSeriousError(_ErrorCode code);	//�˷������õĴ������ϵͳǿ������
	void OutputData(std::string aim, std::string data);
private:
	void __boot(void);	//ϵͳ����
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
	ss << "Inte�ػ��߳�������,id:" << th_Inte.get_id();
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