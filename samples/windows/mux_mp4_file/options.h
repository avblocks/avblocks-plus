#pragma once

#include <vector>

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
    std::vector<std::wstring> input_video;
    std::vector<std::wstring> input_audio;
    std::wstring output_file;

    Options(): help(false), fast_start(false) {}
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
