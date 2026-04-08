#pragma once

#include <primo/avblocks/avb++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <iostream>
#include <sstream>

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

inline void deleteFile(const char* file)
{
    remove(file);
}

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

std::string getExeDir();

