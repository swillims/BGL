#include "staticFile.h"

#include "tinyfiledialogs.h"


bool StaticFile::OpenFile(std::string& path, const char* filter)
{
    const char* filters[] = { filter };

    const char* result = tinyfd_openFileDialog(
        "Open File",
        "",
        filter ? 1 : 0,
        filter ? filters : nullptr,
        nullptr,
        0
    );

    if (result == nullptr)
    {
        return false;
    }

    path = result;
    return true;
}


bool StaticFile::SaveFile(std::string& path, const char* filter)
{
    const char* filters[] = { filter };

    const char* result = tinyfd_saveFileDialog(
        "Save File",
        "",
        filter ? 1 : 0,
        filter ? filters : nullptr,
        nullptr
    );

    if (result == nullptr)
    {
        return false;
    }

    path = result;
    return true;
}