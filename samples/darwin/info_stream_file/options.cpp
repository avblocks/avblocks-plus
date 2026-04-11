#include <string>
#include <iostream>
#include <sstream>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/mov/big_buck_bunny_trailer.mp4";
    cout << "Using default input file.\n";
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "info_stream_file --input <avfile>" << endl;
    doHelp(cout, optcfg);
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile << endl;
        return Parsed;
    }

    OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input,i", opt.inputFile, string(), "file; if no input is specified a default input file is used.");

    try
    {
        scanArgv(optcfg, argc, argv);
    }
    catch (ParseFailure<char>& ex)
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
