#pragma once

#include <primo/avblocks/avb++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

inline void printError(const char* action, const primo::avblocks::modern::TErrorInfo& e)
{
    using namespace std;

    if (action)
        cout << action << ": ";

    if (e.facility() == primo::error::ErrorFacility::Success)
    {
        cout << "Success" << endl;
        return;
    }

    if (!e.message().empty())
        cout << e.message() << ", ";

    cout << "facility:" << e.facility()
         << ", error:" << e.code()
         << ", hint:" << e.hint()
         << endl;
}

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

inline void deleteFile(const char* file)
{
    remove(file);
}

inline std::vector<uint8_t> readFileBytes(const char* name)
{
    std::ifstream f(name, std::ios::binary);
    std::vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, std::ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, std::ios::beg);
        f.read(reinterpret_cast<char*>(&bytes[0]), filesize);
    }
    return bytes;
}

std::string getExeDir();
bool makeDir(const std::string& dir);

