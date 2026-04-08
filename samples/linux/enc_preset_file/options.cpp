/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include <primo/avblocks/avb.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

PresetDescriptor avb_presets[] = {
    
    // video presets
    { Preset::Video::DVD::PAL_4x3_MP2,		"mpg" },
    { Preset::Video::DVD::PAL_16x9_MP2,		"mpg" },
    { Preset::Video::DVD::NTSC_4x3_MP2,		"mpg" },
    { Preset::Video::DVD::NTSC_16x9_MP2,	"mpg" },
    { Preset::Video::AppleTV::H264_480p,	"mp4" },
    { Preset::Video::AppleTV::H264_720p,	"mp4" },
    { Preset::Video::AppleTV::MPEG4_480p,	"mp4" },
    { Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K,           "ts" },
    { Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,	"ts" },
    { Preset::Video::Generic::MP4::Base_H264_AAC,	"mp4" },
    { Preset::Video::iPad::H264_576p,				"mp4" },
    { Preset::Video::iPad::H264_720p,				"mp4" },
    { Preset::Video::iPad::MPEG4_480p,				"mp4" },
    { Preset::Video::iPhone::H264_480p,				"mp4" },
    { Preset::Video::iPhone::MPEG4_480p,			"mp4" },
    { Preset::Video::iPod::H264_240p,				"mp4" },
    { Preset::Video::iPod::MPEG4_240p,				"mp4" },
    { Preset::Video::AndroidPhone::H264_360p,		"mp4" },
    { Preset::Video::AndroidPhone::H264_720p,		"mp4" },
    { Preset::Video::AndroidTablet::H264_720p,		"mp4" },
    { Preset::Video::AndroidTablet::WebM_VP8_720p,	"webm" },
    { Preset::Video::VCD::PAL,						"mpg" },
    { Preset::Video::VCD::NTSC,						"mpg" },
    { Preset::Video::Generic::WebM::Base_VP8_Vorbis,		"webm" }
};

const int avb_presets_len = sizeof(avb_presets) / sizeof(PresetDescriptor);

ColorDescriptor color_formats[] = {
    
    { ColorFormat::YV12,	"yv12",     "Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,	"nv12",     "Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,	"yuy2",     "Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,	"uyvy",     "Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411,	"yuv411",	"Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420,	"yuv420",	"Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422,	"yuv422",	"Planar Y, U, V (4:2:2)" },
    { ColorFormat::YUV444,	"yuv444",	"Planar Y, U, V (4:4:4)" },
    { ColorFormat::Y411,	"y411",     "Composite Y, U, V (4:1:1)" },
    { ColorFormat::Y41P,	"y41p",     "Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,	"bgr32",	"Composite B->G->R" },
	{ ColorFormat::BGRA32,	"bgra32",	"Composite B->G->R->A" },
    { ColorFormat::BGR24,	"bgr24",	"Composite B->G->R" },
    { ColorFormat::BGR565,	"bgr565",	"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
    { ColorFormat::BGR555,	"bgr555",	"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
    { ColorFormat::BGR444,	"bgr444",	"Composite B->G->R->A, 4 bit per component" },
    { ColorFormat::GRAY,	"gray",     "Luminance component only" },
    { ColorFormat::YUV420A,	"yuv420a",	"Planar Y, U, V, Alpha (4:2:0)" },
    { ColorFormat::YUV422A,	"yuv422a",	"Planar Y, U, V, Alpha (4:2:2)" },
    { ColorFormat::YUV444A,	"yuv444a",	"Planar Y, U, V, Alpha (4:4:4)" },
    { ColorFormat::YVU9,	"yvu9",     "Planar Y, V, U, 9 bits per sample" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

PresetDescriptor* getPresetByName(const char* presetName)
{
    for (int i=0; i < avb_presets_len; ++i)
    {
        PresetDescriptor& preset = avb_presets[i];
        if (compareNoCase(preset.name, presetName))
            return &preset;
    }
    return NULL;
}

ColorDescriptor* getColorByName(const char* colorName)
{
    for (int i=0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (compareNoCase(color.name, colorName))
            return &color;
    }
    return NULL;
}

ColorDescriptor* getColorById(primo::codecs::ColorFormat::Enum Id)
{
    for (int i=0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (color.Id == Id)
            return &color;
    }
    return NULL;
}

void listPresets()
{
    cout << "\nPRESETS" << endl;
    cout << "---------" << endl;
    for (int i=0; i < avb_presets_len; ++i)
    {
        const PresetDescriptor& preset = avb_presets[i];
        cout << left << setw(45) <<  preset.name << " ." << preset.extension << endl;
    }
}

void listColors()
{
    cout << "\nCOLORS" << endl;
    cout << "--------" << endl;
    for (int i=0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        cout << left << setw(20) <<  color.name << color.description << endl;
    }
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "\nUsage: enc_preset_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <yuv-file> --output <file> [--preset <PRESET>]";
    cout << " [--colors] [--presets]";
    cout << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    // build output file name "path/foreman_qcif.h264"
    opt.yuv_file = getExeDir() + "/../../assets/vid/foreman_qcif.yuv";

    fs::path output(getExeDir() + "/../../output/enc_preset_file");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/foreman_qcif.mp4";
    opt.output_file = s.str();

    opt.yuv_fps = 30.0;
    opt.yuv_frame.height = 144;
    opt.yuv_frame.width = 176;
    opt.yuv_color = *getColorById(primo::codecs::ColorFormat::YUV420);
    opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
}

bool validateOptions(Options& opt)
{
    if (!opt.preset.name)
    {
        opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
    }
    
    // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
    {
        string::size_type pos = opt.output_file.rfind('.');
        if (0 == pos || string::npos == pos)
        {
            opt.output_file.append(".");
        }
        else
        {
            opt.output_file = opt.output_file.substr(0, pos+1);
        }
        
        opt.output_file.append(opt.preset.extension);
    }
    
    if (opt.yuv_file.empty())
    {
        return false;
    }
    
    if (opt.yuv_frame.width == 0 || opt.yuv_frame.height == 0)
    {
        return false;
    }
    
    if (opt.yuv_color.Id == ColorFormat::Unknown)
    {
        return false;
    }
    
    if (opt.yuv_fps == 0.0)
    {
        return false;
    }
    
    if (opt.output_file.empty())
    {
        return false;
    }
    return true;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.yuv_file;
        cout << " --color " << opt.yuv_color.name;
        cout << " --rate " << opt.yuv_fps;
        cout << " --frame " << opt.yuv_frame.width << "x" << opt.yuv_frame.height;
        cout << " --output " << opt.output_file;
        cout << " --preset " << opt.preset.name;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h",      opt.help,       "")
    ("input,i",     opt.yuv_file,   string(), "input YUV file")
    ("rate,r",      opt.yuv_fps,    0.0,      "input frame rate")
    ("frame,f",     opt.yuv_frame,  FrameSize(), "input frame size, <width>x<height>")
    ("color,c",     opt.yuv_color,  ColorDescriptor(), "input color format. Use --colors to list color formats.")
    ("output,o",    opt.output_file,string(), "output file")
    ("preset,p",    opt.preset,     PresetDescriptor(), "output preset. Use --presets to list presets.")
    ("colors",      opt.list_colors, "list color formats")
    ("presets",     opt.list_presets,"list presets");
    
    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        help(optcfg);
        return Error;
    }
    
    if (opt.help)
    {
        help(optcfg);
        return Command;
    }
    
    if (opt.list_colors)
    {
        listColors();
        return Command;
    }
    
    if (opt.list_presets)
    {
        listPresets();
        return Command;
    }
    
    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }
    
    return Parsed;
}

std::istringstream &operator>>(std::istringstream &in, ColorDescriptor &color)
{
    std::string strColorName;
    in >> strColorName;
    
    ColorDescriptor* colorDesc = getColorByName(strColorName.c_str());
    if(!colorDesc)
        throw primo::program_options::ParseFailure<char>("", strColorName, "Parse error");
    
    color = *colorDesc;
    return in;
}

std::istringstream &operator>>(std::istringstream &in, PresetDescriptor &preset)
{
    std::string presetName;
    in >> presetName;
    
    PresetDescriptor* presetDesc = getPresetByName(presetName.c_str());
    if(!presetDesc)
        throw primo::program_options::ParseFailure<char>("", presetName, "Parse error");
    
    preset = *presetDesc;
    return in;
}

std::istringstream &operator>>(std::istringstream &in, FrameSize &frameSize)
{
    in >> frameSize.width;
    char ch;
    in >> ch;
    in >> frameSize.height;
    return in;
}