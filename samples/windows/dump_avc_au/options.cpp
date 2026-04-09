#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.input_file = getExeDir() + L"/../../assets/vid/foreman_qcif.h264";
    opt.output_dir = getExeDir() + L"/../../output/dump_avc_au";
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"dump_avc_au --input <h264 file> --output <directory>" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !opt.input_file.empty() && !opt.output_dir.empty();
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input "  << opt.input_file;
        wcout << L" --output " << opt.output_dir;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,        L"")
        (L"input,i",  opt.input_file,  wstring(), L"input file (AVC/H.264)")
        (L"output,o", opt.output_dir,  wstring(), L"output directory");

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
