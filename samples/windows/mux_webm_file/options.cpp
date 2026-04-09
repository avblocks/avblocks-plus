#include "stdafx.h"

#include "program_options.h"
#include "util.h"
#include "options.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.input_audio.push_back(getExeDir() + L"/../../assets/aud/big-buck-bunny_trailer_vp8_vorbis.aud.webm");
    opt.input_video.push_back(getExeDir() + L"/../../assets/vid/big-buck-bunny_trailer_vp8_vorbis.vid.webm");

    std::filesystem::path output(getExeDir() + L"/../../output/mux_webm_file");
    std::filesystem::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big-buck-bunny_trailer.webm";
    opt.output_file = s.str();
}

void help(primo::program_options::OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << endl;
    wcout << L"Usage: mux_webm_file --audio <input_Vorbis> --video <input_VP8> --output <output.webm>" << endl;
    doHelp(wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    if (opt.input_audio.empty() && opt.input_video.empty())
        return false;

    if (opt.output_file.empty())
        return false;

    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        for (auto& input : opt.input_audio)
        {
            wcout << L" --audio " << input;
        }
        for (auto& input : opt.input_video)
        {
            wcout << L" --video " << input;
        }
        wcout << L" --output " << opt.output_file;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",   opt.help,        L"")
        (L"audio,a",  opt.input_audio, vector<wstring>(), L"input Vorbis files. Can be used multiple times")
        (L"video,v",  opt.input_video, vector<wstring>(), L"input VP8 files. Can be used multiple times")
        (L"output,o", opt.output_file, wstring(),          L"output file");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t>& ex)
    {
        wcout << ex.message() << endl;
        help(optionsConfig);
        return Error;
    }

    if (opt.help)
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
