#include "pch.h"
#include "PipeIOPromationFactory.h"
#include <iostream>
#include <string>
#include <cstring>

using std::string;

void PipeIOPromationFactory::subfunc_IN_NullDevice(void)
{
	return;
}

void PipeIOPromationFactory::subfunc_IN_StdDevice(void)
{
	using std::getline;
	using std::cin;
	string str;
	getline(cin, str);
	strcpy_s(this->instance->data, PipeBufferSize, str.c_str());
	return;
}

void PipeIOPromationFactory::subfunc_IN_SysFileDevice(void)
{
	return;
}

void PipeIOPromationFactory::subfunc_IN_UserDefinedDevice(void)
{
	if (this->instance->from < device.size()) {
		if (!dev.stand_open && !dev.stream->is_open()) { startupDevice(dev); }
		dev.stream->read(box.data, box.data_size);
		if (!dev.stand_open && dev.stream->is_open()) { dev.stream->close(); }
	}
	else {
		return false;
	}
}
