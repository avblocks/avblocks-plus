#include <iostream>

#include <filesystem>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
namespace fs = std::filesystem;

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: re-encode --input inputFile.mp4 --output outputFile.mp4 [--audio yes|no] [--video yes|no]\n" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/mov/big_buck_bunny_trailer.mp4";

    fs::path output(getExeDir() + "/../../output/re-encode");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/big_buck_bunny_trailer.mp4";
    opt.outputFile = s.str();    
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() &&
            !opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile << endl;
        cout << " --output " << opt.outputFile << endl;
        cout << " --audio " << opt.reEncodeAudio << endl;
        cout << " --video " << opt.reEncodeVideo << endl;
        
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
        ("help,h", opt.help, "")
        ("input,i", opt.inputFile, string(), "input mp4 file")
        ("output,o", opt.outputFile, string(), "output mp4 file")
        ("audio,a", opt.reEncodeAudio, YesNo(true), "re-encode audio, yes|no")
        ("video,v", opt.reEncodeVideo, YesNo(true), "re-encode video, yes|no");
    
    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
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

std::istringstream &operator>>(std::istringstream &in, YesNo &yn)
{
    std::string s;
    in >> s;
    
    if (compareNoCase(s.c_str(), "yes") || compareNoCase(s.c_str(), "y"))
        yn.val = true;
    else if (compareNoCase(s.c_str(), "no") || compareNoCase(s.c_str(), "n"))
        yn.val = false;
    else
        throw std::exception();
    
    return in;
}
