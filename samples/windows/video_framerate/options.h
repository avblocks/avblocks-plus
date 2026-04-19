#pragma once

#include <string>

enum ErrorCodes { Parsed = 0, Error, Command };

struct Options {
    Options(): help(false), frameRate(30.0) {}

    bool help;
    std::wstring inputFile;
    std::wstring outputFile;
    double frameRate;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
