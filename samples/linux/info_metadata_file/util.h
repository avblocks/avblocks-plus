#pragma once

#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <strings.h>

#include <sys/stat.h>
#include <linux/limits.h>

#include <cstring>
#include <print>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>

#include <primo/avblocks/avb++.h>
#include <primo/platform/ustring.h>

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

inline bool makeDir(const std::string& dir)
{
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return !ec;
}

inline std::string getExeDir()
{
    pid_t pid = getpid();

    char proc_link[256];
    sprintf(proc_link, "/proc/%d/exe", pid);

    char exe_path[PATH_MAX];
    int len = readlink(proc_link, exe_path, sizeof(exe_path) - 1);
    if (len > 0)
    {
        exe_path[len] = 0;
    }
    else
    {
        return std::string();
    }

    char* exe_dir = dirname(exe_path);
    return std::string(exe_dir);
}
