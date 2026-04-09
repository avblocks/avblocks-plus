#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/aud/express-dictate_8000_s8_1ch_alaw.wav";

    fs::path output(getExeDir() + L"/../../output/dec_g711_alaw_file");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/express-dictate_8000_s16_1ch_pcm.wav";
    opt.outputFile = s.str();
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"dec_g711_alaw_file --input <g711 alaw wav file> --output <wav file>" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
    (L"help,h", opt.help, L"")
    (L"input,i", opt.inputFile, wstring(), L"input G.711 A-law WAV file")
    (L"output,o", opt.outputFile, wstring(), L"output PCM WAV file");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t> &ex)
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
