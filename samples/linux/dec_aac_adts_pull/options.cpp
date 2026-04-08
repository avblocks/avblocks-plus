#include <primo/avblocks/avb.h>
#include <primo/platform/error_facility.h>

#include "program_options.h"
#include "util.h"
#include "options.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../../assets/aud/Hydrate-Kenny_Beltrey.adts.aac";

    fs::path output(getExeDir() + "/../../output/dec_aac_adts_pull");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/Hydrate-Kenny_Beltrey.wav";
    opt.outputFile = s.str();
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "dec_aac_adts_pull --input <aac file> --output <wav file>" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input,i", opt.inputFile, string(), "input AAC file")
    ("output,o", opt.outputFile, string(), "output WAV file");
    
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
