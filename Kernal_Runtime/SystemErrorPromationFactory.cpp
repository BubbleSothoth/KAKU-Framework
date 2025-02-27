#include "pch.h"
#include "SystemErrorPromationFactory.h"

#pragma comment(lib,"../x64/Debug/Tools.lib")

using std::cout;
using std::endl;

void SystemErrorPromationFactory::create_new(_ErrorCode code)
{
	//((void (*)())PromationFactory::create(code))();
	FuncPoint(funcp) = nullptr;
	funcp = (void (*)(void))PromationFactory::create(code);
	if (funcp != nullptr) {
		funcp();
	}
	return;
}

void SystemErrorPromationFactory::subfunc_FILE_NOT_OPEN_ERROR(void)
{
	cout << "Error : File has not be opened." << endl;
}

void SystemErrorPromationFactory::subfunc_FILE_INPUT_ERROR(void)
{
	LG_ERROR("FILE INPUT ERROR");
}

void SystemErrorPromationFactory::subfunc_MEMORY_ERROR(void)
{
	cout << "Error : Memory error." << endl;
}

void SystemErrorPromationFactory::subfunc_NULL_POINTER_ERROR(void)
{
	LG_ERROR("USED AN NULL POINTER");
}

void SystemErrorPromationFactory::subfunc_DEAD_POINTER_ERROR(void)
{
	LG_ERROR("USED AN DEAD POINTER");
}

void SystemErrorPromationFactory::subfunc_UNKNOW_POINTER_ERROR(void)
{
	LG_ERROR("USED AN POINTER WITHOUT INIT");
}

void SystemErrorPromationFactory::subfunc_NONE_BOOK_OPEN_ERROR(void)
{
	cout << "Error : Please open a book first." << endl;
}

void SystemErrorPromationFactory::subfunc_CONNECT_ERROR(void)
{
	cout << "Error : No network connection." << endl;
}

void SystemErrorPromationFactory::subfunc_UNRESPONSE_ERROR(void)
{
	cout << "Error : No network response." << endl;
}

void SystemErrorPromationFactory::subfunc__SOCKET_ERROR(void)
{
	cout << "Error : Bad Socket." << endl;
}

void SystemErrorPromationFactory::subfunc_BIND_ERROR(void)
{
	cout << "Error : Bad address or port." << endl;
}
