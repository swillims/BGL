#pragma once

#include <string>

struct StaticFile
{
    static bool OpenFile(std::string& path, const char* filter = nullptr);
    static bool SaveFile(std::string& path, const char* filter = nullptr);
};