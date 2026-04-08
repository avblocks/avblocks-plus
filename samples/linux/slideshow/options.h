#pragma once

#include <string>

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct PresetDescriptor
{
    const char * name;
    const char * extension;
};

struct Options
{
    Options() : preset(), help(false), list_presets(false)
    {}

    std::string input_dir;

    // output options
    PresetDescriptor preset;
    std::string output_file;
    
    bool help;
    bool list_presets;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);

