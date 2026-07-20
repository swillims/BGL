#pragma once

#include <fstream>
#include <string>
#include <vector>

struct StaticFile
{
    static std::string NavigateOpenFile(const std::string& title, const std::string& description, const std::string& defaultPath, const std::vector<std::string>& filters);
    static std::string NavigateOpenFile(const std::string& title = "Open", const std::vector<std::string>& filters = {});
    static std::string NavigateSaveFile(const std::string& title, const std::string& description, const std::string& defaultPath, const std::vector<std::string>& filters);
    static std::string NavigateSaveFile(const std::string& title = "Save", const std::vector<std::string>& filters = {});

    static std::string Load(const std::string& path);
    static bool Write(const std::string& path, const std::string& data, bool append = false);

    static bool Exists(const std::string& path);

    static bool Delete(const std::string& path);
};
