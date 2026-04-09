#pragma once

#include <primo/avblocks/avb.h>
#include <string>

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct ColorDescriptor
{
    primo::codecs::ColorFormat::Enum Id;
    const char* name;
    const char* description;
};

class FrameSize
{
public:
    FrameSize() : width(0), height(0) {}
    int width;
    int height;
};

struct Options
{
    Options() : fps(0.0), output_color(), help(false), list_colors(false) {}

    std::wstring input_dir;
    std::wstring output_file;
    FrameSize    frame_size;
    double       fps;
    ColorDescriptor output_color;

    bool help;
    bool list_colors;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
