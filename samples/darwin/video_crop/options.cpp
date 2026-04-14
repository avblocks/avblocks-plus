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
    opt.outputFile = getExeDir() + "/../../output/video_crop/cropped.mp4";
    opt.cropLeft = 60;
    opt.cropRight = 60;
    opt.cropTop = 0;
    opt.cropBottom = 0;

    fs::path output(getExeDir() + "/../../output/video_crop");
    fs::create_directories(output);    
    ostringstream s; 
    s << output.c_str() << "/cropped.mp4";
    opt.outputFile = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: video_crop -i <input.mp4> -o <output.mp4> [--crop-left <left>] [--crop-right <right>] [--crop-top <top>] [--crop-bottom <bottom>]" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << " --crop-left " << opt.cropLeft;
        cout << " --crop-right " << opt.cropRight;
        cout << " --crop-top " << opt.cropTop;
        cout << " --crop-bottom " << opt.cropBottom;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input, i", opt.inputFile, string(), "MP4 input file.")
    ("output, o", opt.outputFile, string(), "MP4 output file.")
    ("crop-left", opt.cropLeft, 60, "Pixels to crop from left.")
    ("crop-right", opt.cropRight, 60, "Pixels to crop from right.")
    ("crop-top", opt.cropTop, 0, "Pixels to crop from top.")
    ("crop-bottom", opt.cropBottom, 0, "Pixels to crop from bottom.");
    
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
    
    return Parsed;
}
