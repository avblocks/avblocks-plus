#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.input_files.push_back(getExeDir() + "/../../assets/mov/big_buck_bunny_trailer.mp4");
    opt.fast_start = true;

    fs::path output(getExeDir() + "/../../output/mux_mp4_fs_file");
    fs::create_directories(output);

    ostringstream s;
    s << output.c_str() << "/big_buck_bunny_trailer_fast_start.mp4";
    opt.output_file = s.str();
}

void help(primo::program_options::OptionsConfig<char>& optionsConfig)
{
    cout << endl;
    cout << "Usage: mux_mp4_fs_file --input <file1> --input <file2> --output <output.mp4> [--fast_start]" << endl;
    doHelp(cout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    if (opt.input_files.empty())
    {
        return false;
    }

    if (opt.output_file.empty())
    {
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
        for (const auto& input : opt.input_files)
        {
            cout << " --input " << input;
        }
        cout << " --output " << opt.output_file;
        if (opt.fast_start)
        {
            cout << " --fast_start";
        }
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
        ("help,?",      opt.help,                               "")
        ("fast_start,f", opt.fast_start,                        "move the moov atom to the beginning of the file (fast start)")
        ("input,i",     opt.input_files,    vector<string>(),   "input file (AAC, H264, MP4). Can be used multiple times")
        ("output,o",    opt.output_file,    string(),           "output file");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        help(optionsConfig);
        return Error;
    }

    if(opt.help)
    {
        help(optionsConfig);
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

    return Parsed;
}