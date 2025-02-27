#pragma once
#include <cstdlib>
#include <cstring>

class IOContorller
{
public:
	enum IOSelection
	{
		both,
		in,
		out
	};
private:
	enum IOContorller::IOSelection io_sel;
public:
	IOContorller(void) = default;
	IOContorller(enum IOSelection sel) :
		io_sel(sel){}
	~IOContorller(void) = default;;
public:
	enum IOSelection GetIoSelection(void) { return this->io_sel; }
	bool IsInMode(void) { return this->io_sel == IOSelection::in; }
	bool IsOutMode(void) { return this->io_sel == IOSelection::out; }
	bool IsIOMode(void){ return this->io_sel == IOSelection::both; }
	bool InAccess(void){ return this->io_sel == IOSelection::in || this->io_sel == IOSelection::both; }
	bool OutAccess(void){ return this->io_sel == IOSelection::out || this->io_sel == IOSelection::both; }
};

