#pragma once

#include "../DLLAPI.h"

#include "pch.h"
#include "../Tools/Score.h"

//RST��
#define RSTSIZE 0X100U
#define RSTCode unsigned short

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
#define ExitReadyFlag			0x12			//����ʱϵͳExit������������RuntimeSystem::Exit������ϣ�
#define InputBufReadyFlag		0x13			//���������־���˱�־Ϊtrue˵�����뻺������׼���ý��ն�������ݣ���ҪIO��ϵͳ���ж���
#define OutputBufReadyFlag		0x14			//���������־���˱�־Ϊtrue˵������Ҫ��������ݲ����������������׼���ã���ҪIO��ϵͳ�������
#define InputFinishedFlag		0x15			//������ɱ�־���˱�־Ϊtrue˵��IO��ϵͳ�Ѿ���ɶ�������������Ѿ�����¼�����뻺������
#define OutputFinishedFlag		0x16			//������ɱ�־���˱�־Ϊtrue˵��IO��ϵͳ�Ѿ�����������
// ���Ը�����Ҫ����Զ���RST���ֵ��ע�����Ĭ��RTS�����֧��255���Ӧ�±�0x00-0xFF���������˷�Χ��Ҫ���¶���RSTSIZE�ꡣ
// ����˳�����RTS����ı�ţ������������µĿռ��˷ѣ���ΪRST�����ڴ�����˳��洢�ģ�Ҳ�������飬RTS����ı��ʵ���Ͼ�����
// �ڴ��и��ݻ���ַ��ƫ�ƣ��±꣩�����磬���Ϊ0x1000(4096)�ı�����ζ��RST��������4097��Ԫ�ء�

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
	static Score<bool[RSTSIZE]> runningStateTableScore;//RST��
	static Score<std::stack<_ErrorCode>> systemErrorTableScore;//SET��
	std::mutex ioMutex;		//IO�߳�����Ϊ��IOʱ������ͬ��
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
	[[deprecated("�����ø÷�������ʹ��LOCK()�����")]]
	void __Lock(std::mutex& mtx) = delete;
	[[deprecated("�����ø÷�������ʹ��UNLOCK()�����")]]
	void __Unlock(std::mutex& mtx) = delete;
public:
	RuntimeSystem(void);
	RuntimeSystem(const RuntimeSystem&) = delete;	//��ֹ����
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
	[[noinline]] static void setNormalError(_ErrorCode code);	//�˷�����������һ���ԵĴ���
	[[noinline]] static void setSeriousError(_ErrorCode code);	//�˷������õĴ������ϵͳǿ������
	void InputData(std::string aim, std::string& data);
	void InputData(std::string aim, BYTE* data, size_t data_size);
	void OutputData(std::string aim, std::string data);
	struct CommandStruct GetNextCommand(void);
private:
	void __boot(void);	//ϵͳ����
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
		ss << "Inte�ػ��߳�������,id:" << th_Inte->get_id();
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
	//�����������Exit�����������и����Exit����
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