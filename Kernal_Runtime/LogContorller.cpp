#include "pch.h"
#include "LogContorller.h"
#include <cstdio>

LogContorller::LogContorller(const char* log_file):
    logFile(nullptr)
{
    RegistLogFile(log_file);
}

LogContorller::~LogContorller(void)
{
    free(this->logFile);
    if(this->logfc!=nullptr)
        delete this->logfc;
}  

void LogContorller::RegistLogFile(const char* log_file)
{
    if (this->logFile != nullptr) free(this->logFile);
    this->logFile = (char*)malloc(strlen(log_file) + 1);
    if (this->logFile == nullptr) return;
    strcpy_s(this->logFile, strlen(log_file) + 1, log_file);
    this->logfc = new BufferFileContorller(IOContorller::IOSelection::out, Tools::AnsiToWChar(this->logFile).c_str());
}

bool LogContorller::Record(LIP* info)
{
    bool rtn;
    const size_t OutStrMaxSize = 1024 + 512;
    char* out = (char*)calloc(OutStrMaxSize, sizeof(char)); //out长度1.5KB，比LIR多大约70B.
    if (out == nullptr)
        rtn = false;
    else {
        this->__generate__logstr(out, OutStrMaxSize, info);
        rtn = this->logfc->Append(out, strlen(out));
        free(out);
    }
    return rtn;
}

void LogContorller::__generate__logstr(char* buf, size_t buf_size, LIP* info)
{
    if (buf == nullptr) return;
    sprintf_s(buf, buf_size, "[%s] %s %s [%s : %d] %s : %s\n", info->type, info->date, info->time, info->file, info->line, info->func, info->mess);
    return;
}

void InitLogInfoStruct(LIP* buf,
    const char* type,
    const char* mess,
    const char* file,
    const char* func,
    int line)
{
    if (buf == nullptr) return;

    ZeroMemory(buf, sizeof(LIP));

    file = file == nullptr ? "--" : file;
    func = func == nullptr ? "--" : func;

    strcpy_s(buf->type, sizeof(buf->type), type);
    strcpy_s(buf->mess, sizeof(buf->mess), mess);
    strcpy_s(buf->file, sizeof(buf->file), file);
    strcpy_s(buf->func, sizeof(buf->func), func);
    //strcpy_s(buf->line, sizeof(buf->line), line);
    buf->line = line;

    time_t t = time(NULL);
    struct tm tm;
    localtime_s(&tm, &t);
    strftime(buf->date, sizeof(buf->date), "%F %a", &tm);
    strftime(buf->time, sizeof(buf->time), "%H:%M:%S", &tm);

    return;
}
