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
    Options() : fps(0.0), help(false), list_colors(false) {}

    std::wstring yuv_file;
    std::wstring h264_file;
    FrameSize    frame_size;
    ColorDescriptor yuv_color;
    double       fps;

    bool help;
    bool list_colors;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
