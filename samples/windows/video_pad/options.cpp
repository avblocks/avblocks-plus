#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/vid/big_buck_bunny_trailer.vid.mp4";
    opt.width = 1920;
    opt.height = 1080;
    opt.padLeft = 100;
    opt.padRight = 100;
    opt.padTop = 100;
    opt.padBottom = 100;
    opt.padColor = 0xFF000000;

    fs::path output(getExeDir() + L"/../../output/video_pad");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big_buck_bunny_padded.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: video_pad -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>] [-l <left>] [-r <right>] [-t <top>] [-b <bottom>] [-c <color>]" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.width == 0 || opt.height == 0)
    {
        wcout << L"Error: width and height must be specified" << endl;
        return false;
    }
    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << L" --width " << opt.width;
        wcout << L" --height " << opt.height;
        wcout << L" --left " << opt.padLeft;
        wcout << L" --right " << opt.padRight;
        wcout << L" --top " << opt.padTop;
        wcout << L" --bottom " << opt.padBottom;
        wcout << L" --color " << opt.padColor;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
    (L"help,h", opt.help, L"")
    (L"input, i", opt.inputFile, wstring(), L"MP4 input file.")
    (L"output, o", opt.outputFile, wstring(), L"MP4 output file.")
    (L"width, w", opt.width, 1920, L"Target width (pixels).")
    (L"height, h", opt.height, 1080, L"Target height (pixels).")
    (L"left, l", opt.padLeft, 100, L"Left padding (pixels).")
    (L"right, r", opt.padRight, 100, L"Right padding (pixels).")
    (L"top, t", opt.padTop, 100, L"Top padding (pixels).")
    (L"bottom, b", opt.padBottom, 100, L"Bottom padding (pixels).")
    (L"color, c", opt.padColor, 0xFF000000U, L"Padding color (ARGB32).");

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

    if (!validateOptions(opt))
    {
        return Error;
    }

    return Parsed;
}
