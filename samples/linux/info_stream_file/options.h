#pragma once

#include <string>

enum ErrorCodes { Parsed = 0, Error, Command };

struct Options {
    Options() : help(false) {}
    std::string inputFile;
    bool help;
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);
