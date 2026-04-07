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

ColorDescriptor color_formats[] = {
    
    { ColorFormat::YV12,	"yv12",     "Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,	"nv12",     "Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,	"yuy2",     "Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,	"uyvy",     "Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411,	"yuv411",   "Planar Y, U, V (4:1:1)" },
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

void setDefaultOptions(Options& opt)
{
    // build output file name "path/foreman_qcif.ivf"
    opt.yuv_file = getExeDir() + "/../../assets/vid/foreman_qcif.yuv";

    fs::path output(getExeDir() + "/../../output/enc_vp9_file");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/foreman_qcif.ivf";
    opt.ivf_file = s.str();
    
    opt.frame_size.width_ = 176;
    opt.frame_size.height_ = 144;
    opt.fps = 30.0;
    opt.yuv_color = *getColorById(ColorFormat::YUV420);
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf>" << endl
    << "[--colors]" << endl;
    doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if(opt.yuv_file.empty())
    {
        cout << "input file needed" << endl;
        return false;
    }
    else
    {
        cout << "input file: " << opt.yuv_file << endl;
    }
    
    if(opt.ivf_file.empty())
    {
        cout << "output file needed" << endl;
        return false;
    }
    else
    {
        cout << "output file: " << opt.ivf_file << endl;
    }
    
    if(opt.fps <= 0.0)
    {
        cout << "Invalid frame rate" << endl;
        return false;
    }
    else
    {
        cout << "frame rate: " << opt.fps << endl;
    }
    
    if(opt.frame_size.width_ <= 0 || opt.frame_size.height_ <= 0)
    {
        cout << "Invalid frame size/s" << endl;
        return false;
    }
    else
    {
        cout << "frame size: " << opt.frame_size.width_ << "x" << opt.frame_size.height_ << endl;
    }
    
    if(opt.yuv_color.Id == ColorFormat::Unknown)
    {
        cout << "Missing or incorrect color format" << endl;
        return false;
    }
    else
    {
        cout << "color format: " << opt.yuv_color.name << endl;
    }
    
    return true;
}

void listColors()
{
    cout << "\nCOLORS:" << endl;
    cout << "--------------------------------------" << endl;
    for (int i=0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        cout << left << setw(20) <<  color.name << color.description << endl;
    }
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << "enc_vp9_file --input " << opt.yuv_file;
        cout << " --output " << opt.ivf_file;
        cout << " --rate " << opt.fps;
        cout << " --frame " << opt.frame_size.width_ << "x" << opt.frame_size.height_;
        cout << " --color " << opt.yuv_color.name;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    (("help,h"),	opt.help,					(""))
    (("input,i"),	opt.yuv_file,           string(),		("input YUV file"))
    (("output,o"),	opt.ivf_file,           string(),		("output IVF file"))
    (("rate,r"),	opt.fps,                0.0,			("input frame rate"))
    (("frame,f"),	opt.frame_size,         FrameSize(),		("input frame sizes <width>x<height>"))
    (("color,c"),	opt.yuv_color,          ColorDescriptor(),	("input color format. Use --colors to list all supported color formats"))
    (("colors"),	opt.list_colors,				("list COLOR constants"));
    
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
    
    if(opt.list_colors)
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

std::istringstream &operator>>(std::istringstream &in, FrameSize &frameSize)
{
    in >> frameSize.width_;
    
    char ch;

    in >> ch;
    
    in >> frameSize.height_;
    return in;
}

std::istringstream &operator>>(std::istringstream &in, ColorDescriptor &colorDesc)
{
    std::string strColorName;
    in >> strColorName;
    
    ColorDescriptor* pc = getColorByName(strColorName.c_str());
    if(!pc)
        throw primo::program_options::ParseFailure<char>((""), strColorName, ("Parse error"));
    
    colorDesc = *pc;
    
    return in;
}
