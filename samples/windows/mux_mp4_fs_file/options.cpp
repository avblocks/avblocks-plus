#include "stdafx.h"

#include "program_options.h"
#include "util.h"
#include "options.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.input_files.push_back(getExeDir() + L"/../../assets/mov/big_buck_bunny_trailer.mp4");
    opt.fast_start = true;

    fs::path output(getExeDir() + L"/../../output/mux_mp4_fs_file");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big_buck_bunny_trailer_fast_start.mp4";
    opt.output_file = s.str();
}

void help(primo::program_options::OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << endl;
    wcout << L"Usage: mux_mp4_fs_file --input <file1> --input <file2> --output <output.mp4> [--fast_start]" << endl;
    doHelp(wcout, optionsConfig);
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

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        for (const auto& input : opt.input_files)
        {
            wcout << L" --input " << input;
        }
        wcout << L" --output " << opt.output_file;
        if (opt.fast_start)
        {
            wcout << L" --fast_start";
        }
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",       opt.help,                                L"")
        (L"fast_start,f", opt.fast_start,                          L"move the moov atom to the beginning of the file (fast start)")
        (L"input,i",      opt.input_files,    vector<wstring>(),   L"input file (AAC, H264, MP4). Can be used multiple times")
        (L"output,o",     opt.output_file,    wstring(),           L"output file");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t> &ex)
    {
        wcout << ex.message() << endl;
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
