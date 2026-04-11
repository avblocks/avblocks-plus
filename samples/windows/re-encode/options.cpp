#include "stdafx.h"

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: re-encode --input inputFile.mp4 --output outputFile.mp4 [--audio yes|no] [--video yes|no]\n" << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"/../../assets/mov/big_buck_bunny_trailer.mp4";

    std::filesystem::path output(getExeDir() + L"/../../output/re-encode");
    std::filesystem::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/big_buck_bunny_trailer.mp4";
    opt.outputFile = s.str();
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() && !opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile << endl;
        wcout << L" --output " << opt.outputFile << endl;
        wcout << L" --audio " << opt.reEncodeAudio << endl;
        wcout << L" --video " << opt.reEncodeVideo << endl;
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,          L"")
        (L"input,i",  opt.inputFile,     wstring(),   L"input mp4 file")
        (L"output,o", opt.outputFile,    wstring(),   L"output mp4 file")
        (L"audio,a",  opt.reEncodeAudio, YesNo(true), L"re-encode audio, yes|no")
        (L"video,v",  opt.reEncodeVideo, YesNo(true), L"re-encode video, yes|no");

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

std::wistringstream& operator>>(std::wistringstream& in, YesNo& yn)
{
    std::wstring ws;
    in >> ws;

    if (0 == _wcsicmp(ws.c_str(), L"yes") || 0 == _wcsicmp(ws.c_str(), L"y"))
        yn.val = true;
    else if (0 == _wcsicmp(ws.c_str(), L"no") || 0 == _wcsicmp(ws.c_str(), L"n"))
        yn.val = false;
    else
        throw std::exception();

    return in;
}
