#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libproc.h>

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

std::string getExeDir() {
    char path_buf[PROC_PIDPATHINFO_MAXSIZE] = {0};

    pid_t pid = (pid_t) getpid();
    int ret = proc_pidpath (pid, path_buf, sizeof(path_buf));
    if (ret <= 0) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    }    

    std::string dir = fs::path(path_buf).parent_path().c_str();
    return dir;
}
