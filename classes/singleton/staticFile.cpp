#include "staticFile.h"

#include <filesystem>

#include "tinyfiledialogs.h"

std::string StaticFile::NavigateOpenFile(const std::string& title, const std::string& description, const std::string& defaultPath, const std::vector<std::string>& filters)
{
    // convert some data
    std::vector<const char*> cFilters;
    for (const std::string& filter : filters)
    {
        cFilters.push_back(filter.c_str());
    }

    // ugly tinyfd call
    const char* result = tinyfd_openFileDialog(
        title.c_str(),
        defaultPath.c_str(),
        static_cast<int>(cFilters.size()),
        cFilters.empty() ? nullptr : cFilters.data(),
        description.empty() ? nullptr : description.c_str(),
        false // allow multiple selections
    );

    if (result == nullptr)
    {
        return "";
    }

    return result;
}
std::string StaticFile::NavigateOpenFile(const std::string& title, const std::vector<std::string>& filters)
{
    return NavigateOpenFile(title, "", "", filters);
}

std::string StaticFile::NavigateSaveFile(const std::string& title, const std::string& description, const std::string& defaultPath, const std::vector<std::string>& filters)
{
    // convert some data
    std::vector<const char*> cFilters;
    for (const std::string& filter : filters)
    {
        cFilters.push_back(filter.c_str());
    }

    // ugly tinyfd call
    const char* result = tinyfd_saveFileDialog(
        title.c_str(),
        defaultPath.c_str(),
        static_cast<int>(cFilters.size()),
        cFilters.empty() ? nullptr : cFilters.data(),
        description.empty() ? nullptr : description.c_str()
    );

    if (result == nullptr)
    {
        return "";
    }

    return result;
}
std::string StaticFile::NavigateSaveFile(const std::string& title, const std::vector<std::string>& filters)
{
    return NavigateSaveFile(title, "", "", filters);
}

std::string StaticFile::Load(const std::string &path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return "";
    }

    std::string data;
    data.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    return data;
}

bool StaticFile::Write(const std::string &path, const std::string &data, bool append)
{
    {
        std::ofstream file
        (
            path, append ? std::ios::app : std::ios::out
        );

        if (!file.is_open())
        {
            return false;
        }

        file << data;

        file.close();
        return true;
    }
}

bool StaticFile::Exists(const std::string &path)
{
    std::ifstream file(path);
    return file.good();
}

bool StaticFile::Delete(const std::string &path)
{
    return std::filesystem::remove(path);
}

