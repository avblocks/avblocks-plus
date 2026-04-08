/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */
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

struct ColorDescriptor
{
    primo::codecs::ColorFormat::Enum Id;
    const char * name;
    const char * description;
};

struct FrameSize
{
    int width;
    int height;
};

struct Options
{
    Options() :
    preset(), yuv_frame(),yuv_fps(0.0), yuv_color(),
    help(false), list_colors(false), list_presets(false)
    {}
    
    // output options
    PresetDescriptor preset;
    std::string output_file;
    
    // input options
    FrameSize yuv_frame;
    double	yuv_fps;
    ColorDescriptor yuv_color;
    std::string yuv_file;
    
    bool help;
    bool list_colors;
    bool list_presets;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);

