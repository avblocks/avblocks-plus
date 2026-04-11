#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

ColorDescriptor color_formats[] = {
    { ColorFormat::YV12,   L"yv12",   L"Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,   L"nv12",   L"Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,   L"yuy2",   L"Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,   L"uyvy",   L"Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411, L"yuv411", L"Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420, L"yuv420", L"Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422, L"yuv422", L"Planar Y, U, V (4:2:2)" },
    { ColorFormat::YUV444, L"yuv444", L"Planar Y, U, V (4:4:4)" },
    { ColorFormat::Y411,   L"y411",   L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::Y41P,   L"y41p",   L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,  L"bgr32",  L"Composite B->G->R" },
    { ColorFormat::BGRA32, L"bgra32", L"Composite B->G->R->A" },
    { ColorFormat::BGR24,  L"bgr24",  L"Composite B->G->R" },
    { ColorFormat::BGR565, L"bgr565", L"Composite B->G->R, 5/6/5 bits" },
    { ColorFormat::BGR555, L"bgr555", L"Composite B->G->R->A, 5 bits each" },
    { ColorFormat::BGR444, L"bgr444", L"Composite B->G->R->A, 4 bits each" },
    { ColorFormat::GRAY,   L"gray",   L"Luminance component only" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

ColorDescriptor* getColorByName(const wchar_t* colorName)
{
    for (int i = 0; i < color_formats_len; ++i)
        if (0 == _wcsicmp(color_formats[i].name, colorName))
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
    opt.yuv_file = getExeDir() + L"/../../assets/vid/foreman_qcif.yuv";

    fs::path output(getExeDir() + L"/../../output/enc_hevc_file");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/foreman_qcif.h265";
    opt.h265_file = s.str();

    opt.frame_size.width  = 176;
    opt.frame_size.height = 144;
    opt.fps = 30.0;
    opt.yuv_color = *getColorById(ColorFormat::YUV420);
}

void listColors()
{
    wcout << L"\nCOLORS:\n--------------------------------------\n";
    for (int i = 0; i < color_formats_len; ++i)
        wcout << left << setw(20) << color_formats[i].name << color_formats[i].description << L"\n";
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"enc_hevc_file --frame <width>x<height> --rate <fps> --color <COLOR> "
             L"--input <file.yuv> --output <file.h265> [--colors]\n";
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.yuv_file.empty())   { wcout << L"input file needed\n";  return false; }
    if (opt.h265_file.empty())  { wcout << L"output file needed\n"; return false; }
    if (opt.fps <= 0.0)         { wcout << L"invalid frame rate\n"; return false; }
    if (opt.frame_size.width <= 0 || opt.frame_size.height <= 0)
                                { wcout << L"invalid frame size\n"; return false; }
    if (opt.yuv_color.Id == ColorFormat::Unknown)
                                { wcout << L"missing color format\n"; return false; }
    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input "  << opt.yuv_file;
        wcout << L" --output " << opt.h265_file;
        wcout << L" --rate "   << opt.fps;
        wcout << L" --frame "  << opt.frame_size.width << L"x" << opt.frame_size.height;
        wcout << L" --color "  << opt.yuv_color.name;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,           L"")
        (L"input,i",  opt.yuv_file,       wstring(),          L"input YUV file")
        (L"output,o", opt.h265_file,      wstring(),          L"output H.265 file")
        (L"rate,r",   opt.fps,            0.0,                L"input frame rate")
        (L"frame,f",  opt.frame_size,     FrameSize(),        L"frame size <width>x<height>")
        (L"color,c",  opt.yuv_color,      ColorDescriptor(),  L"input color format (use --colors to list)")
        (L"colors",   opt.list_colors,                        L"list COLOR constants");

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

    if (opt.help)        { help(optcfg);   return Command; }
    if (opt.list_colors) { listColors();   return Command; }
    if (!validateOptions(opt)) { help(optcfg); return Error; }

    return Parsed;
}

std::wistringstream& operator>>(std::wistringstream& in, ColorDescriptor& color)
{
    std::wstring wname;
    in >> wname;
    ColorDescriptor* cd = getColorByName(wname.c_str());
    if (!cd)
        throw primo::program_options::ParseFailure<wchar_t>(L"", wname, L"Parse error");
    color = *cd;
    return in;
}

std::wistringstream& operator>>(std::wistringstream& in, FrameSize& fs)
{
    in >> fs.width;
    wchar_t ch; in >> ch;
    in >> fs.height;
    return in;
}
