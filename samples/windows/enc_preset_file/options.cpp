#include "stdafx.h"

/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

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
    
    { ColorFormat::YV12,	L"yv12",     L"Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,	L"nv12",     L"Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,	L"yuy2",     L"Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,	L"uyvy",     L"Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411,	L"yuv411",	L"Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420,	L"yuv420",	L"Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422,	L"yuv422",	L"Planar Y, U, V (4:2:2)" },
    { ColorFormat::YUV444,	L"yuv444",	L"Planar Y, U, V (4:4:4)" },
    { ColorFormat::Y411,	L"y411",     L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::Y41P,	L"y41p",     L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,	L"bgr32",	L"Composite B->G->R" },
    { ColorFormat::BGRA32,	L"bgra32",	L"Composite B->G->R->A" },
    { ColorFormat::BGR24,	L"bgr24",	L"Composite B->G->R" },
    { ColorFormat::BGR565,	L"bgr565",	L"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
    { ColorFormat::BGR555,	L"bgr555",	L"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
    { ColorFormat::BGR444,	L"bgr444",	L"Composite B->G->R->A, 4 bit per component" },
    { ColorFormat::GRAY,	L"gray",     L"Luminance component only" },
    { ColorFormat::YUV420A,	L"yuv420a",	L"Planar Y, U, V, Alpha (4:2:0)" },
    { ColorFormat::YUV422A,	L"yuv422a",	L"Planar Y, U, V, Alpha (4:2:2)" },
    { ColorFormat::YUV444A,	L"yuv444a",	L"Planar Y, U, V, Alpha (4:4:4)" },
    { ColorFormat::YVU9,	L"yvu9",     L"Planar Y, V, U, 9 bits per sample" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

PresetDescriptor* getPresetByName(const wchar_t* presetName)
{
    for (int i = 0; i < avb_presets_len; ++i)
    {
        PresetDescriptor& preset = avb_presets[i];
        if (0 == _wcsicmp(toWide(preset.name).c_str(), presetName))
            return &preset;
    }
    return NULL;
}

ColorDescriptor* getColorByName(const wchar_t* colorName)
{
    for (int i = 0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (0 == _wcsicmp(color.name, colorName))
            return &color;
    }
    return NULL;
}

ColorDescriptor* getColorById(primo::codecs::ColorFormat::Enum Id)
{
    for (int i = 0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (color.Id == Id)
            return &color;
    }
    return NULL;
}

void listPresets()
{
    using namespace std;
    wcout << L"\nPRESETS" << endl;
    wcout << L"---------" << endl;
    for (int i = 0; i < avb_presets_len; ++i)
    {
        const PresetDescriptor& preset = avb_presets[i];
        wcout << left << setw(45) << toWide(preset.name) << L" ." << toWide(preset.extension) << endl;
    }
}

void listColors()
{
    using namespace std;
    wcout << L"\nCOLORS" << endl;
    wcout << L"--------" << endl;
    for (int i = 0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        wcout << left << setw(20) << color.name << color.description << endl;
    }
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    using namespace std;
    wcout << L"\nUsage: enc_preset_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <yuv-file> --output <file> [--preset <PRESET>]";
    wcout << L" [--colors] [--presets]";
    wcout << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.yuv_file = getExeDir() + L"/../../assets/vid/foreman_qcif.yuv";

    std::filesystem::path output(getExeDir() + L"/../../output/enc_preset_file");
    std::filesystem::create_directories(output);

    std::wostringstream s;
    s << output.c_str() << L"/foreman_qcif.mp4";
    opt.output_file = s.str();

    opt.yuv_fps = 30.0;
    opt.yuv_frame.height = 144;
    opt.yuv_frame.width = 176;
    opt.yuv_color = *getColorById(primo::codecs::ColorFormat::YUV420);
    opt.preset = *getPresetByName(toWide(Preset::Video::Generic::MP4::Base_H264_AAC).c_str());
}

bool validateOptions(Options& opt)
{
    if (!opt.preset.name)
    {
        opt.preset = *getPresetByName(toWide(Preset::Video::Generic::MP4::Base_H264_AAC).c_str());
    }

    // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
    {
        std::wstring::size_type pos = opt.output_file.rfind(L'.');
        if (0 == pos || std::wstring::npos == pos)
        {
            opt.output_file.append(L".");
        }
        else
        {
            opt.output_file = opt.output_file.substr(0, pos + 1);
        }

        opt.output_file.append(toWide(opt.preset.extension));
    }

    if (opt.yuv_file.empty())
        return false;

    if (opt.yuv_frame.width == 0 || opt.yuv_frame.height == 0)
        return false;

    if (opt.yuv_color.Id == ColorFormat::Unknown)
        return false;

    if (opt.yuv_fps == 0.0)
        return false;

    if (opt.output_file.empty())
        return false;

    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        std::wcout << L"Using defaults:\n";
        std::wcout << L" --input " << opt.yuv_file;
        std::wcout << L" --color " << opt.yuv_color.name;
        std::wcout << L" --rate " << opt.yuv_fps;
        std::wcout << L" --frame " << opt.yuv_frame.width << L"x" << opt.yuv_frame.height;
        std::wcout << L" --output " << opt.output_file;
        std::wcout << L" --preset " << toWide(opt.preset.name);
        std::wcout << std::endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",      opt.help,        L"")
        (L"input,i",     opt.yuv_file,    std::wstring(), L"input YUV file")
        (L"rate,r",      opt.yuv_fps,     0.0,            L"input frame rate")
        (L"frame,f",     opt.yuv_frame,   FrameSize(),    L"input frame size, <width>x<height>")
        (L"color,c",     opt.yuv_color,   ColorDescriptor(), L"input color format. Use --colors to list color formats.")
        (L"output,o",    opt.output_file, std::wstring(), L"output file")
        (L"preset,p",    opt.preset,      PresetDescriptor(), L"output preset. Use --presets to list presets.")
        (L"colors",      opt.list_colors, L"list color formats")
        (L"presets",     opt.list_presets, L"list presets");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t>& ex)
    {
        std::wcout << ex.message() << std::endl;
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

std::wistringstream& operator>>(std::wistringstream& in, ColorDescriptor& color)
{
    std::wstring wname;
    in >> wname;

    ColorDescriptor* colorDesc = getColorByName(wname.c_str());
    if (!colorDesc)
        throw primo::program_options::ParseFailure<wchar_t>(L"", wname, L"Parse error");

    color = *colorDesc;
    return in;
}

std::wistringstream& operator>>(std::wistringstream& in, PresetDescriptor& preset)
{
    std::wstring wname;
    in >> wname;

    PresetDescriptor* presetDesc = getPresetByName(wname.c_str());
    if (!presetDesc)
        throw primo::program_options::ParseFailure<wchar_t>(L"", wname, L"Parse error");

    preset = *presetDesc;
    return in;
}

std::wistringstream& operator>>(std::wistringstream& in, FrameSize& frameSize)
{
    in >> frameSize.width;
    wchar_t ch;
    in >> ch;
    in >> frameSize.height;
    return in;
}
