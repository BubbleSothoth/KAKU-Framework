#pragma once

//ERRORCode
#define FILE_NOT_OPEN_ERROR		0x00
#define FILE_INPUT_ERROR		0x01
#define FILE_OUTPUT_ERROR		0x02
#define MEMORY_ERROR			0x10
#define NULL_POINTER_ERROR		0x20
#define DEAD_POINTER_ERROR		0x21
#define UNKNOWN_POINTER_ERROR	0x22
#define CONNECT_ERROR			0x40
#define UNRESPONSE_ERROR		0x41
#define _SOCKET_ERROR			0x42
#define BIND_ERROR				0x43
#define NONE_ERROR_OBJECT		0xFD
#define LIB_ERROR				0xFE
#define NO_ERROR				0xFF

#ifndef _ErrorCode
#define _ErrorCode unsigned short
#include <climits>
#define MaxErrorNumber USHRT_MAX
#endif // !_ErrorCode