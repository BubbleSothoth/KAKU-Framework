#include "pch.h"
#include "PluginSystem.h"
#include <iostream>
#include "../Tools/Tools.h"

PluginSystem::PluginSystem(std::string path):
	srcPath(path)
{
    InstallPlugin();
}

PluginSystem::~PluginSystem(void)
{
    for (int i = 0; i < pluginList.size(); i++) {
        delete pluginList.Access();
    }
    pluginList.clear();
}

void PluginSystem::setsrcPath(std::string path) {
    this->srcPath = path;
}

Plugin* PluginSystem::AccessPlugin(void) {
    if (this->pluginList.empty()) {
        return nullptr;
    }
    return this->pluginList.Access();
}

void PluginSystem::InstallPlugin(void) {
    std::vector<std::string> files;
    files = __get__all__dll__files(this->srcPath);
    for (auto it = files.begin(); it != files.end(); it++) {
        this->pluginList.insert(new Plugin(Tools::AnsiToWChar(*it)));
    }
    return;
}

std::vector<std::string> PluginSystem::__get__all__dll__files(const std::string& path) {
    std::vector<std::string> dllFiles;
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA((path + "*.dll").c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                dllFiles.push_back(path + findData.cFileName);
            }
        } while (FindNextFileA(hFind, &findData) != 0);
        FindClose(hFind);
    }
    return dllFiles;
}
