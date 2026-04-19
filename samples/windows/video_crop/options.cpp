#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/vid/big_buck_bunny_trailer.vid.mp4";
    opt.cropLeft = 60;
    opt.cropRight = 60;
    opt.cropTop = 0;
    opt.cropBottom = 0;

    fs::path output(getExeDir() + L"/../../output/video_crop");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/cropped.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: video_crop -i <input.mp4> -o <output.mp4> [--crop-left <left>] [--crop-right <right>] [--crop-top <top>] [--crop-bottom <bottom>]" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << L" --crop-left " << opt.cropLeft;
        wcout << L" --crop-right " << opt.cropRight;
        wcout << L" --crop-top " << opt.cropTop;
        wcout << L" --crop-bottom " << opt.cropBottom;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
    (L"help,h", opt.help, L"")
    (L"input, i", opt.inputFile, wstring(), L"MP4 input file.")
    (L"output, o", opt.outputFile, wstring(), L"MP4 output file.")
    (L"crop-left", opt.cropLeft, 60, L"Pixels to crop from left.")
    (L"crop-right", opt.cropRight, 60, L"Pixels to crop from right.")
    (L"crop-top", opt.cropTop, 0, L"Pixels to crop from top.")
    (L"crop-bottom", opt.cropBottom, 0, L"Pixels to crop from bottom.");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t> &ex)
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

    return Parsed;
}
