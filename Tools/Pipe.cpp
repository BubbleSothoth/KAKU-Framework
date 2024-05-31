#include "pch.h"
#include "Pipe.h"
#include <Windows.h>
#include <direct.h>
#include <iostream>
#include <cstring>
#include <cstring>
#include <mutex>
#include <string>
#include <fstream>

#include "PipePromationFactory.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

std::vector<struct Device> Pipe::device;
std::queue<struct DataBox> Pipe::msg_queue;

Pipe::Pipe(void)
{
	Pipe_AUTOLOCK(this->buf_mtx, this->dev_mtx);
	this->msg_queue;
	this->device.resize(5);
	this->device[NullDevice] = { NullDevice,"",nullptr,FreeRW };
	this->device[StdDevice] = { StdDevice,"",nullptr,FreeRW };
	auto _ = _mkdir("SysFile");
	this->device[TempFile] = {
		TempFile,
		"temp.tmp",
		new std::fstream("SysFile\\temp.tmp"),
		FreeRW
	};
	this->device[OperationLogFile] = {
		OperationLogFile,
		"operation.log",
		new std::fstream("SysFile\\operation.log", std::ios::out | std::ios::app),
		__WriteOnly
	};
	this->device[DebugLogFile] = {
		DebugLogFile,
		"debug.log",
		new std::fstream("SysFile\\debug.log",std::ios::out | std::ios::app),
		__WriteOnly
	};
}

Pipe::~Pipe(void) {
	Pipe_AUTOLOCK()
	for (auto it = this->device.begin(); it != this->device.end(); it++) {
		uninstallDevice(it);	//传参为迭代器的uninstallDevice方法需要手动加锁(因为它是给系统内部使用的私有方法)
	}
}

void Pipe::write(DeviceId aim, const char* data)
{
	struct DataBox box = {
		0,
		OutputProperty,
		aim,
		(char*)data,
		strlen(data),
		false
	};
	__setBuffer(box);
	return;
}

void Pipe::read(DeviceId from, char* buff, size_t size, size_t buff_size)
{
	struct DataBox box = {
	0,
	InputProperty,
	from,
	buff,
	size,
	false
	};
	__setBuffer(box);
	//while (!box.IO_successful);	//等待IO完毕
	return;
}

DeviceId Pipe::installDevice(std::string file, RWMode mode,bool stamd_open)
{

	struct Device temp = creatDevice(file, mode, stamd_open);
	__setDevice(temp);
	return temp.id;
}

//此函数不能用于卸载系统保留设备
void Pipe::uninstallDevice(DeviceId id) {
	PipeDev_AUTOLOCK();
	struct Device& dev = device[id];
	if (id < 5) {
		return;
	}
	if (dev.stream != nullptr && dev.stream->is_open()) {
		dev.stream->close();
	}
	delete dev.stream;
	dev.stream = nullptr;
	dev.id = UINT16_MAX;
	dev.mode = Unopreationable;
	return;
}

//使用此方法前需要手动加锁
void Pipe::uninstallDevice(std::vector<Device>::iterator it) {
	if (it->stream != nullptr && it->stream->is_open()) {
		it->stream->close();
	}
	delete it->stream;
	it->stream = nullptr;
	it->id = UINT16_MAX;
	it->mode = Unopreationable;
	return;
}

struct Device Pipe::creatDevice(std::string file, RWMode mode, bool stand_open) {
	PipeDev_AUTOLOCK();
	return { 
		(DeviceId)(this->device.back().id + 1),
		file,
		new std::fstream(),
		mode
	};
}

//使用此方法前需要对device手动加锁
void Pipe::startupDevice(DeviceId id) {
	startupDevice(device[id]);
}

//使用此方法前需要对device手动加锁
void Pipe::startupDevice(Device& dev) {
	using std::ios;
	ios::ios_base::open_mode _mode = ios::_Openmode();
	switch (device.back().mode)
	{
	case __ReadOnly:
		_mode = ios::in;
		break;
	case __WriteOnly:
		_mode = ios::out;
		break;
	case FreeRW:
		_mode = ios::in | ios::out;
		break;
	default:
		break;
	}
	dev.stream->open(dev.file, _mode);
	return;
}

void Pipe::closeDevice(DeviceId id)
{
	closeDevice(device[id]);
	return;
}

void Pipe::closeDevice(Device& dev)
{
	dev.stream->close();
	return;
}

bool Pipe::__substep__Input(struct DataBox& box, size_t buff_size)
{
	Device& dev = device[box.aim];
	//const Device& dev = __getDevice(box.aim);
	string str;
	switch (box.aim)
	{
	case NullDevice:
		break;
	case StdDevice:
		std::getline(cin, str);
		strcpy_s(box.data, buff_size, str.c_str());
		break;
	case TempFile:
	case OperationLogFile:
	case DebugLogFile:
		break;
	default:
		if (box.from < device.size()) {
			startupDevice(dev);
			dev.stream->read(box.data, box.data_size);
			closeDevice(dev);
		}
		else {
			return false;
		}
	}
	return true;
}

bool Pipe::__substep__Output(DataBox& box)
{
	Device& dev = device[box.aim];
	switch (box.aim)
	{
	case NullDevice:
		break;
	case StdDevice:
		cout << string(box.data) << endl;
		break;
	case TempFile:
	case OperationLogFile:
	case DebugLogFile:
		dev.stream->write(box.data, strlen(box.data));
		break;
	default:
		return __substep__Output__UserDefinedDevice(box, dev);
	}
	return true;
}

/*此方法仅被__substep__Output调用*/
bool Pipe::__substep__Output__UserDefinedDevice(DataBox& box, Device& dev)
{
	if (box.aim < device.size()) {
		startupDevice(dev);
		dev.stream->write(box.data, strlen(box.data));
		closeDevice(dev);
	}
	else {
		return false;
	}
	return true;
}

void Pipe::__subfunc_Input_Sptcial(DataBox& box)
{
	switch (box.aim)
	{
	case NullDevice:
		break;
	case StdDevice:
		std::cin.getline(box.data, box.data_size);
		break;
	default:
		break;
	}
}

void Pipe::__subfunc_Input_File(DataBox& box)
{
	Device& dev = this->device[box.aim];
	memset(box.data, 0, sizeof(char) * box.data_size);
	startupDevice(dev);
	dev.stream->read(box.data, sizeof(char) * box.data_size);
	closeDevice(dev);
}

void Pipe::__subfunc_Input_Socket(DataBox& box)
{
}

/*'w'为写入，'r'为读出,'d'为删除,'c'为修改*/
Device* Pipe::__optDevice(char mode, Device* dev, DeviceId id)
{
	PipeDev_AUTOLOCK();
	Device* rtn = nullptr;
	if (mode == 'w') {
		this->device.push_back(*dev);
	}
	else if(mode == 'r') {
		if (id >= this->device.size()) { rtn = nullptr; }
		else { rtn = &this->device[id]; }
	}
	else if (mode=='d') {
		if (id < this->device.size()) { 
			this->device.erase(device.begin() + id); 
		}
	}
	else {
		if (id < this->device.size()) {
			rtn = dev;
		}
		else {
			rtn = nullptr;
		}
		this->device[id] = *dev;
	}
	return rtn;
}

const Device& Pipe::__getDevice(DeviceId id)
{
	return *__optDevice('r', nullptr, id);
}

void Pipe::__setDevice(Device& dev)
{
	__optDevice('w', &dev, 0);
	return;
}

/*'w'为写入，'r'为读出*/
DataBox* Pipe::__optBuffer(char mode, DataBox* data)
{
	PipeBuf_AUTOLOCK();
	DataBox* rtn = nullptr;
	if (mode == 'w') {
		this->msg_queue.push(*data); 
	}
	else { 
		if (this->msg_queue.empty()) { 
			rtn == nullptr;
		}
		else {
			rtn = &(this->msg_queue.back());
			this->msg_queue.pop();
		}
	}
	return rtn;
}

const DataBox& Pipe::__getBuffer(void)
{
	return *__optBuffer('r', nullptr);
}

void Pipe::__setBuffer(DataBox& lpbox)
{
	__optBuffer('w', &lpbox);
	return;
}

void Pipe::mainProgress(void) {
	Pipe_AUTOLOCK();
	unsigned int operation_code = 0x00000000;
	while (!msg_queue.empty()) {
		IO_MESSAGE& msg = msg_queue.front();
		msg_queue.pop();
		operation_code = (msg.IOProperty) << 8 * sizeof(bool) + msg.aim << 8 * sizeof(DeviceId);	//生成操作码
		PipePromationFactory ppf;
		ppf.create_new(operation_code);
	}
	return;
}

void __substep_input(DataBox& box) {
	switch (GetDeviceType(box.aim))
	{
	case SptcialDeviceType:
		break;
	case FileDeviceType:
		break;
	case SocketDeviceType:
		break;
	case DefauleDeviceType:
		break;
	default:
		break;
	}
	return;
}