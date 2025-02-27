#pragma once
#include "BufferFileContorller.h"
#include "..\debug.h"
#include <cstring>

typedef struct LogInfoReport {
	char type[16];
	char date[16];
	char time[16];
	char file[260];
	char func[128];
	int line;
	char mess[1024];
} LIP;

void InitLogInfoStruct(LIP* buf, const char* type, const char* mess, const char* file = nullptr, const char* func = nullptr, int line = 0);


class LogContorller
{
private:
	char* logFile = nullptr;
	BufferFileContorller* logfc = nullptr;
public:
	LogContorller(void) = default;
	LogContorller(const LogContorller&) = delete;
	LogContorller(const char* log_file);
	~LogContorller(void);
	void RegistLogFile(const char* log_file);
	bool Record(LIP* info);
private:
	void __generate__logstr(char* buf, size_t buf_size, LIP* info);
};