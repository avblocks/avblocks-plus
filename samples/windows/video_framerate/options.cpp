#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/vid/big_buck_bunny_trailer.vid.mp4";
    opt.frameRate = 30.0;

    fs::path output(getExeDir() + L"/../../output/video_framerate");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big_buck_bunny_30fps.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: video_framerate -i <mp4 file> -o <mp4 file> [-f <fps>]" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.frameRate <= 0.0)
    {
        wcout << L"Error: Frame rate must be positive" << endl;
        return false;
    }

    return !(opt.inputFile.empty() || opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << L" --frame-rate " << opt.frameRate;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
    (L"help,h", opt.help, L"")
    (L"input, i", opt.inputFile, wstring(), L"MP4 input file.")
    (L"output, o", opt.outputFile, wstring(), L"MP4 output file.")
    (L"frame-rate, f", opt.frameRate, 30.0, L"Target frame rate (fps).");

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
        help(optcfg);
        return Error;
    }

    return Parsed;
}
