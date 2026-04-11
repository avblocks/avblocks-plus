#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/aud/Hydrate-Kenny_Beltrey.ogg";
    wcout << L"Using default option: info_metadata_file --input " << opt.inputFile << endl << endl;
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"info_metadata_file --input <file>" << endl;
    doHelp(wcout, optcfg);
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        return Parsed;
    }

    OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
    (L"help,h", opt.help, L"")
    (L"input,i", opt.inputFile, wstring(), L"file; if no input is specified a default input file is used.");

    try
    {
        scanArgv(optcfg, argc, argv);
    }
    catch (ParseFailure<wchar_t>& ex)
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
