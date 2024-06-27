#include "pch.h"
#include "Plugin.h"

Plugin::Plugin(std::wstring file) :
    file(file)
{
    hDLL = LoadLibrary(file.c_str());
    return;
}

Plugin::~Plugin(void)
{
    FreeLibrary(hDLL);
}

void Plugin::Call(std::string func_name)
{
    if (!IsCallable(func_name)) {
        //printf("%d\n", GetLastError());
        return;
    }
    (ExampleFunction)GetProcAddress(hDLL, func_name.c_str())();
    return;
}

bool Plugin::IsCallable(std::string func_name)
{
    //printf_s("%p\n", GetProcAddress(hDLL, func_name.c_str()));
    return GetProcAddress(hDLL, func_name.c_str()) != nullptr;
}

bool Plugin::IsInstalled(void)
{
    return this->hDLL != nullptr;
}


