#pragma once


#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <strings.h>

#include <sys/stat.h>
#include <linux/limits.h>

#include <string>
#include <iostream>

#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

inline void printError(const char* action, const primo::error::ErrorInfo* e)
{
    using namespace std;

    if (action)
    {
        std::cout << action << ": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success" << endl;
        return;
    }

    if (e->message())
    {
        cout << primo::ustring(e->message()) << " ";
    }
    
    cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
}

inline std::string getExeDir()
{	
   /*
     Linux:
    /proc/<pid>/exe

    Solaris:
    /proc/<pid>/object/a.out (filename only)
    /proc/<pid>/path/a.out (complete pathname)

    BSD:
    /proc/<pid>/file

    */

    pid_t pid = getpid();
    
    char proc_link[256];
    sprintf(proc_link,"/proc/%d/exe",pid);

    char exe_path[PATH_MAX];
    int len = readlink(proc_link, exe_path, sizeof(exe_path) - 1);
    if(len > 0)
    {
        exe_path[len] = 0;
    }
    else
    {
        return std::string();
    }
    
    char * exe_dir = dirname(exe_path);    
    return std::string(exe_dir);
}

inline void deleteFile(const char* file)
{
    remove(file);
}


