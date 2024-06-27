#pragma once
#include "../Tools/PromationFactory.h"
#include "System.h"

class DLLAPI SystemErrorPromationFactory :
	public PromationFactory<_ErrorCode, void*>
{
public:
	SystemErrorPromationFactory(void) {
		Add(FILE_NOT_OPEN_ERROR, &subfunc_FILE_NOT_OPEN_ERROR);
		Add(FILE_INPUT_ERROR, &subfunc_FILE_INPUT_ERROR);
		Add(MEMORY_ERROR, &subfunc_MEMORY_ERROR);
		Add(NULL_POINTER_ERROR, &subfunc_NULL_POINTER_ERROR);
		Add(DEAD_POINTER_ERROR, &subfunc_DEAD_POINTER_ERROR);
		Add(UNKNOW_POINTER_ERROR, &subfunc_UNKNOW_POINTER_ERROR);
		Add(NONE_BOOK_OPEN_ERROR, &subfunc_NONE_BOOK_OPEN_ERROR);
		Add(CONNECT_ERROR, &subfunc_CONNECT_ERROR);
		Add(UNRESPONSE_ERROR, subfunc_UNRESPONSE_ERROR);
		Add(_SOCKET_ERROR, &subfunc__SOCKET_ERROR);
		Add(BIND_ERROR, &subfunc_BIND_ERROR);
	}
	SystemErrorPromationFactory(const SystemErrorPromationFactory&) = delete;
	~SystemErrorPromationFactory(void) = default;
	void create_new(_ErrorCode);
private:
	static void subfunc_FILE_NOT_OPEN_ERROR(void);
	static void subfunc_FILE_INPUT_ERROR(void);
	static void subfunc_MEMORY_ERROR(void);
	static void subfunc_NULL_POINTER_ERROR(void);
	static void subfunc_DEAD_POINTER_ERROR(void);
	static void subfunc_UNKNOW_POINTER_ERROR(void);
	static void subfunc_NONE_BOOK_OPEN_ERROR(void);
	static void subfunc_CONNECT_ERROR(void);
	static void subfunc_UNRESPONSE_ERROR(void);
	static void subfunc__SOCKET_ERROR(void);
	static void subfunc_BIND_ERROR(void);
};

