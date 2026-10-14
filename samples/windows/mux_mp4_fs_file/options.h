#pragma once

#include <vector>
#include <string>

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct Options
{
    bool help;
    bool fast_start;
    std::vector<std::wstring> input_files;
    std::wstring output_file;

    Options(): help(false), fast_start(false) {}
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
