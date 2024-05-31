#pragma once
#include "../Reader/DLLAPI.h"

#include <string>

typedef void (*ExampleFunction)(void);

class DLLAPI Plugin
{
private:
	std::wstring file;
	HINSTANCE hDLL = nullptr;
public:
	Plugin(void) = delete;
	Plugin(std::wstring file);
	Plugin(const Plugin&) = delete;
	~Plugin(void);
	void Call(std::string);
	bool IsCallable(std::string);
	bool IsInstalled(void);
};

