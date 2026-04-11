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

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

ColorDescriptor color_formats[] = {

    { ColorFormat::YV12,    L"yv12",     L"Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,    L"nv12",     L"Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,    L"yuy2",     L"Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,    L"uyvy",     L"Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411,  L"yuv411",   L"Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420,  L"yuv420",   L"Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422,  L"yuv422",   L"Planar Y, U, V (4:2:2)" },
    { ColorFormat::YUV444,  L"yuv444",   L"Planar Y, U, V (4:4:4)" },
    { ColorFormat::Y411,    L"y411",     L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::Y41P,    L"y41p",     L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,   L"bgr32",    L"Composite B->G->R" },
    { ColorFormat::BGRA32,  L"bgra32",   L"Composite B->G->R->A" },
    { ColorFormat::BGR24,   L"bgr24",    L"Composite B->G->R" },
    { ColorFormat::BGR565,  L"bgr565",   L"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
    { ColorFormat::BGR555,  L"bgr555",   L"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
    { ColorFormat::BGR444,  L"bgr444",   L"Composite B->G->R->A, 4 bit per component" },
    { ColorFormat::GRAY,    L"gray",     L"Luminance component only" },
    { ColorFormat::YUV420A, L"yuv420a",  L"Planar Y, U, V, Alpha (4:2:0)" },
    { ColorFormat::YUV422A, L"yuv422a",  L"Planar Y, U, V, Alpha (4:2:2)" },
    { ColorFormat::YUV444A, L"yuv444a",  L"Planar Y, U, V, Alpha (4:4:4)" },
    { ColorFormat::YVU9,    L"yvu9",     L"Planar Y, V, U, 9 bits per sample" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

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

void setDefaultOptions(Options& opt)
{
    opt.yuv_file = getExeDir() + L"/../../assets/vid/foreman_qcif.yuv";

    std::filesystem::path output(getExeDir() + L"/../../output/enc_vp9_file");
    std::filesystem::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/foreman_qcif.ivf";
    opt.ivf_file = s.str();

    opt.frame_size.width_ = 176;
    opt.frame_size.height_ = 144;
    opt.fps = 30.0;
    opt.yuv_color = *getColorById(ColorFormat::YUV420);
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>" << endl
          << L"[--colors]" << endl;
    doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.yuv_file.empty())
    {
        wcout << L"input file needed" << endl;
        return false;
    }
    else
    {
        wcout << L"input file: " << opt.yuv_file << endl;
    }

    if (opt.ivf_file.empty())
    {
        wcout << L"output file needed" << endl;
        return false;
    }
    else
    {
        wcout << L"output file: " << opt.ivf_file << endl;
    }

    if (opt.fps <= 0.0)
    {
        wcout << L"Invalid frame rate" << endl;
        return false;
    }
    else
    {
        wcout << L"frame rate: " << opt.fps << endl;
    }

    if (opt.frame_size.width_ <= 0 || opt.frame_size.height_ <= 0)
    {
        wcout << L"Invalid frame size/s" << endl;
        return false;
    }
    else
    {
        wcout << L"frame size: " << opt.frame_size.width_ << L"x" << opt.frame_size.height_ << endl;
    }

    if (opt.yuv_color.Id == ColorFormat::Unknown)
    {
        wcout << L"Missing or incorrect color format" << endl;
        return false;
    }
    else
    {
        wcout << L"color format: " << opt.yuv_color.name << endl;
    }

    return true;
}

void listColors()
{
    wcout << L"\nCOLORS:" << endl;
    wcout << L"--------------------------------------" << endl;
    for (int i = 0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        wcout << left << setw(20) << color.name << color.description << endl;
    }
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"enc_vp9_file --input " << opt.yuv_file;
        wcout << L" --output " << opt.ivf_file;
        wcout << L" --rate " << opt.fps;
        wcout << L" --frame " << opt.frame_size.width_ << L"x" << opt.frame_size.height_;
        wcout << L" --color " << opt.yuv_color.name;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,        L"")
        (L"input,i",  opt.yuv_file,    wstring(),          L"input YUV file")
        (L"output,o", opt.ivf_file,    wstring(),          L"output IVF file")
        (L"rate,r",   opt.fps,         0.0,                L"input frame rate")
        (L"frame,f",  opt.frame_size,  FrameSize(),        L"input frame sizes <width>x<height>")
        (L"color,c",  opt.yuv_color,   ColorDescriptor(),  L"input color format. Use --colors to list all supported color formats")
        (L"colors",   opt.list_colors, L"list COLOR constants");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t>& ex)
    {
        wcout << ex.message() << endl;
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

    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }

    return Parsed;
}

std::wistringstream& operator>>(std::wistringstream& in, FrameSize& frameSize)
{
    in >> frameSize.width_;
    wchar_t ch;
    in >> ch;
    in >> frameSize.height_;
    return in;
}

std::wistringstream& operator>>(std::wistringstream& in, ColorDescriptor& colorDesc)
{
    std::wstring wname;
    in >> wname;

    ColorDescriptor* pc = getColorByName(wname.c_str());
    if (!pc)
        throw primo::program_options::ParseFailure<wchar_t>(L"", wname, L"Parse error");

    colorDesc = *pc;
    return in;
}
