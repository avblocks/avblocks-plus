#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "util.h"
#include "options.h"
#include "program_options.h"

namespace fs = std::filesystem;

using namespace std;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/vid/big_buck_bunny_trailer.vid.mp4";
    opt.frameRate = 30.0;

    fs::path output(getExeDir() + "/../../output/video_framerate");
    fs::create_directories(output);

    ostringstream s;
    s << output.c_str() << "/big_buck_bunny_30fps.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: video_framerate -i <mp4 file> -o <mp4 file> [-f <fps>]" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.frameRate <= 0.0)
    {
        cout << "Error: Frame rate must be positive" << endl;
        return false;
    }

    return !(opt.inputFile.empty() || opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << " --frame-rate " << opt.frameRate;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input, i", opt.inputFile, string(), "MP4 input file.")
    ("output, o", opt.outputFile, string(), "MP4 output file.")
    ("frame-rate, f", opt.frameRate, 30.0, "Target frame rate (fps).");

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
