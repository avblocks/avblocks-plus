#pragma once

#include <primo/avblocks/avb.h>
#include <string>

enum ErrorCodes
{
    Parsed = 0,
    Error,
    Command,
};

struct PresetDescriptor
{
    const char* name;
    const char* extension;
};

struct ColorDescriptor
{
    primo::codecs::ColorFormat::Enum Id;
    const wchar_t* name;
    const wchar_t* description;
};

struct FrameSize
{
    int width;
    int height;
};

struct Options
{
    Options() :
        preset(), yuv_frame(), yuv_fps(0.0), yuv_color(),
        help(false), list_colors(false), list_presets(false)
    {}

    // output options
    PresetDescriptor preset;
    std::wstring output_file;

    // input options
    FrameSize yuv_frame;
    double    yuv_fps;
    ColorDescriptor yuv_color;
    std::wstring yuv_file;

    bool help;
    bool list_colors;
    bool list_presets;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);
