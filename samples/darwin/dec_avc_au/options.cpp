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
    { ColorFormat::YV12,   "yv12",   "Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,   "nv12",   "Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,   "yuy2",   "Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,   "uyvy",   "Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411, "yuv411", "Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420, "yuv420", "Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422, "yuv422", "Planar Y, U, V (4:2:2)" },
    { ColorFormat::Y41P,   "y41p",   "Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,  "bgr32",  "Composite B->G->R" },
    { ColorFormat::BGRA32, "bgra32", "Composite B->G->R->A" },
    { ColorFormat::BGR24,  "bgr24",  "Composite B->G->R" },
    { ColorFormat::BGR565, "bgr565", "Composite B->G->R, 5/6/5 bits" },
    { ColorFormat::BGR555, "bgr555", "Composite B->G->R->A, 5 bits each" },
    { ColorFormat::BGR444, "bgr444", "Composite B->G->R->A, 4 bits each" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

ColorDescriptor* getColorByName(const char* colorName)
{
    for (int i = 0; i < color_formats_len; ++i)
        if (compareNoCase(color_formats[i].name, colorName))
            return &color_formats[i];
    return nullptr;
}

ColorDescriptor* getColorById(ColorFormat::Enum id)
{
    for (int i = 0; i < color_formats_len; ++i)
        if (color_formats[i].Id == id)
            return &color_formats[i];
    return nullptr;
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir   = getExeDir() + "/../../assets/vid/foreman_qcif.h264.au/";
    opt.output_color = *getColorById(primo::codecs::ColorFormat::YUV420);
}

void listColors()
{
    cout << "\nCOLORS:\n-------\n";
    for (int i = 0; i < color_formats_len; ++i)
        cout << left << setw(20) << color_formats[i].name << color_formats[i].description << "\n";
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "dec_avc_au --input <directory> [--output <file>] "
            "[--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]\n";
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !opt.input_dir.empty();
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_dir << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
        ("help,h",   opt.help,            "")
        ("input,i",  opt.input_dir,        string(),          "input directory (AU sequence)")
        ("output,o", opt.output_file,      string(),          "output YUV file")
        ("rate,r",   opt.fps,              0.0,               "frame rate")
        ("frame,f",  opt.frame_size,       FrameSize(),       "frame size <width>x<height>")
        ("color,c",  opt.output_color,     ColorDescriptor(), "output color format (use --colors to list)")
        ("colors",   opt.list_colors,                         "list COLOR formats");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char>& ex)
    {
        cout << ex.message() << endl;
        help(optcfg);
        return Error;
    }

    if (opt.help)        { help(optcfg);   return Command; }
    if (opt.list_colors) { listColors();   return Command; }
    if (!validateOptions(opt)) { help(optcfg); return Error; }

    return Parsed;
}

std::istringstream& operator>>(std::istringstream& in, ColorDescriptor& color)
{
    std::string name;
    in >> name;
    ColorDescriptor* cd = getColorByName(name.c_str());
    if (!cd)
        throw primo::program_options::ParseFailure<char>("", name, "Parse error");
    color = *cd;
    return in;
}

std::istringstream& operator>>(std::istringstream& in, FrameSize& fs)
{
    in >> fs.width;
    char ch; in >> ch;
    in >> fs.height;
    return in;
}
