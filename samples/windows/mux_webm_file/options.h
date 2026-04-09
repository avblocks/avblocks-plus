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
    std::vector<std::wstring> input_video;
    std::vector<std::wstring> input_audio;
    std::wstring output_file;

    Options(): help(false) {}
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
