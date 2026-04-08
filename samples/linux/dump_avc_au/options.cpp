#include <primo/avblocks/avb.h>

#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>

#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.input_file = getExeDir() + "/../../assets/vid/foreman_qcif.h264";
    opt.output_dir = getExeDir() + "/../../output/dump_avc_au";
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "dump_avc_au --input <h264 file> --output <directory>" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !opt.input_file.empty() && !opt.output_dir.empty();
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input "  << opt.input_file;
        cout << " --output " << opt.output_dir;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
        ("help,h",   opt.help,        "")
        ("input,i",  opt.input_file,  string(), "input file (AVC/H.264)")
        ("output,o", opt.output_dir,  string(), "output directory");

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
