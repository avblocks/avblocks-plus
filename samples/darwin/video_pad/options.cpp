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
    opt.padLeft = 100;
    opt.padRight = 100;
    opt.padTop = 100;
    opt.padBottom = 100;
    opt.padColor = 0xFF000000;

    fs::path output(getExeDir() + "/../../output/video_pad");
    fs::create_directories(output);

    ostringstream s;
    s << output.c_str() << "/big_buck_bunny_padded.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: video_pad -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>] [-l <left>] [-r <right>] [-t <top>] [-b <bottom>] [-c <color>]" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if (opt.width == 0 || opt.height == 0)
    {
        cout << "Error: width and height must be specified" << endl;
        return false;
    }
    return true;
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
        cout << " --left " << opt.padLeft;
        cout << " --right " << opt.padRight;
        cout << " --top " << opt.padTop;
        cout << " --bottom " << opt.padBottom;
        cout << " --color " << opt.padColor;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input, i", opt.inputFile, string(), "MP4 input file.")
    ("output, o", opt.outputFile, string(), "MP4 output file.")
    ("width, w", opt.width, 1920, "Target width (pixels).")
    ("height, h", opt.height, 1080, "Target height (pixels).")
    ("left, l", opt.padLeft, 100, "Left padding (pixels).")
    ("right, r", opt.padRight, 100, "Right padding (pixels).")
    ("top, t", opt.padTop, 100, "Top padding (pixels).")
    ("bottom, b", opt.padBottom, 100, "Bottom padding (pixels).")
    ("color, c", opt.padColor, 0xFF000000U, "Padding color (ARGB32).");

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
