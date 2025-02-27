#pragma once

#include "../DLLAPI.h"
#include "pch.h"

#include <list>
#include <string>
#include <queue>
#include "Plugin.h"
#include "../Tools/CircularQueue.h"

class DLLAPI PluginSystem
{
private:
	CircularQueue<Plugin*> pluginList;
	std::string srcPath;
public:
	PluginSystem(void) = default;
	PluginSystem(std::string path);
	PluginSystem(const PluginSystem&) = delete;
	~PluginSystem(void);
	void setsrcPath(std::string path);
	Plugin* AccessPlugin(void);
	void InstallPlugin(void);
private:
    std::vector<std::string> __get__all__dll__files(const std::string& path);
};