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
    opt.width = 1920;
    opt.height = 1080;

    fs::path output(getExeDir() + "/../../output/video_upscale");
    fs::create_directories(output);

    ostringstream s;
    s << output.c_str() << "/big_buck_bunny_1080p.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: video_upscale -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>]" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.width <= 0 || opt.height <= 0)
    {
        cout << "Error: Width and height must be positive" << endl;
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
        cout << " --width " << opt.width;
        cout << " --height " << opt.height;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input, i", opt.inputFile, string(), "MP4 input file.")
    ("output, o", opt.outputFile, string(), "MP4 output file.")
    ("width, w", opt.width, 1920, "Target width (pixels).")
    ("height, h", opt.height, 1080, "Target height (pixels).");

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
        return Error;
    }

    return Parsed;
}
