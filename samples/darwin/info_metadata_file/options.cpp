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
    opt.inputFile = getExeDir() + "/../../assets/aud/Hydrate-Kenny_Beltrey.ogg";
    cout << "Using default option: info_metadata_file --input " << opt.inputFile << endl << endl;
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "info_metadata_file --input <file>" << endl;
    doHelp(cout, optcfg);
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
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
