#pragma once
#include "PromationFactory.h"
#include "Pipe.h"
class PipeIOPromationFactory :
	public PromationFactory<DeviceId, void (PipeIOPromationFactory::*)(void), struct DataBox>
{
private:
	/*
	IOFlag:
	true——Read
	false——Write
	*/
	bool IOFlag;
public:
	PipeIOPromationFactory(struct DataBox* lpbox, bool IOFlag):
		PromationFactory(lpbox), IOFlag(IOFlag)
	{
		Add(NullDevice, &PipeIOPromationFactory::subfunc_IN_NullDevice);
		Add(StdDevice, &PipeIOPromationFactory::subfunc_IN_StdDevice);
		Add(TempFile, &PipeIOPromationFactory::subfunc_IN_SysFileDevice);
		Add(OperationLogFile, &PipeIOPromationFactory::subfunc_IN_SysFileDevice);
		Add(DebugLogFile, &PipeIOPromationFactory::subfunc_IN_SysFileDevice);
	}
	PipeIOPromationFactory(const PipeIOPromationFactory&) = delete;
	~PipeIOPromationFactory(void) = default;
	//这是一个已经被放弃的方法（尽管它继承自父类），请使用create_new()来代替它
	inline volatile void* create(std::string) = delete;
	void create_new(std::string);
private:
	void subfunc_IN_NullDevice(void);
	void subfunc_IN_StdDevice(void);
	void subfunc_IN_SysFileDevice(void);
	void subfunc_IN_UserDefinedDevice(void);
};

