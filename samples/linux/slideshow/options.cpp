#include <iterator>
#include <primo/avblocks/avb.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

PresetDescriptor avb_presets[] = {
    // video presets
    { Preset::Video::DVD::PAL_4x3_MP2,                                                  "mpg" },
    { Preset::Video::DVD::PAL_16x9_MP2,                                                 "mpg" },
    { Preset::Video::DVD::NTSC_4x3_MP2,                                                 "mpg" },
    { Preset::Video::DVD::NTSC_16x9_MP2,                                                "mpg" },
    { Preset::Video::AppleTV::H264_480p,                                                "mp4" },
    { Preset::Video::AppleTV::H264_720p,                                                "mp4" },
    { Preset::Video::AppleTV::MPEG4_480p,                                               "mp4" },
    { Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K,           "ts" },
    { Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,	"ts" },
    { Preset::Video::Generic::MP4::Base_H264_AAC,                                       "mp4" },
    { Preset::Video::iPad::H264_576p,                                                   "mp4" },
    { Preset::Video::iPad::H264_720p,                                                   "mp4" },
    { Preset::Video::iPad::MPEG4_480p,                                                  "mp4" },
    { Preset::Video::iPhone::H264_480p,                                                 "mp4" },
    { Preset::Video::iPhone::MPEG4_480p,                                                "mp4" },
    { Preset::Video::iPod::H264_240p,                                                   "mp4" },
    { Preset::Video::iPod::MPEG4_240p,                                                  "mp4" },
    { Preset::Video::AndroidPhone::H264_360p,                                           "mp4" },
    { Preset::Video::AndroidPhone::H264_720p,                                           "mp4" },
    { Preset::Video::AndroidTablet::H264_720p,                                          "mp4" },
    { Preset::Video::AndroidTablet::WebM_VP8_720p,                                      "webm" },
    { Preset::Video::VCD::PAL,                                                          "mpg" },
    { Preset::Video::VCD::NTSC,                                                         "mpg" },
    { Preset::Video::Generic::WebM::Base_VP8_Vorbis,                                    "webm" }
};

const int avb_presets_len = sizeof(avb_presets) / sizeof(PresetDescriptor);

void listPresets()
{
    cout << "\nPRESETS" << endl;
    cout << "-------" << endl;
    for (int i=0; i < avb_presets_len; ++i)
    {
        const PresetDescriptor& preset = avb_presets[i];
        cout << left << setw(45) <<  preset.name << " ." << preset.extension << endl;
    }
}

PresetDescriptor* getPresetByName(const char* presetName)
{
    for (int i=0; i< avb_presets_len; ++i)
    {
        PresetDescriptor* preset = &avb_presets[i];
        if (0 == strcasecmp(preset->name, presetName))
            return preset;
    }
    return NULL;
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "\nUsage: slideshow --input <directory> --output <file> [--preset <PRESET>]";
    cout << " [--presets]";
    cout << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir = getExeDir() + "/../../assets/img";

    fs::path output(getExeDir() + "/../../output/slideshow");
    fs::create_directories(output);    

    ostringstream s; 
    s << output.c_str() << "/cube.mp4";
    opt.output_file = s.str();

    opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
}

bool validateOptions(Options& opt)
{
    if (!opt.preset.name)
    {
        opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
    }
    
    // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
    {
        string::size_type pos = opt.output_file.rfind('.');
        if (0 == pos || string::npos == pos)
        {
            opt.output_file.append(".");
        }
        else
        {
            opt.output_file = opt.output_file.substr(0, pos+1);
        }
        
        opt.output_file.append(opt.preset.extension);
    }
    
    if (opt.output_file.empty())
    {
        return false;
    }

    return true;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_dir;
        cout << " --output " << opt.output_file;
        cout << " --preset " << opt.preset.name;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h",      opt.help,       "")
    ("input,i",  opt.input_dir, string(), "input directory containing images for the slideshow.")
    ("output,o", opt.output_file, string(), "output filename (without extension). The extension is added based on the preset.")
    ("preset,p", opt.preset, PresetDescriptor(), "output preset id. Use --presets to list presets.")
    ("presets",  opt.list_presets,"list presets");

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
    
    if (opt.list_presets)
    {
        listPresets();
        return Command;
    }
    
    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }
    
    return Parsed;
}

std::istringstream &operator>>(std::istringstream &in, PresetDescriptor &preset)
{
    std::string presetName;
    in >> presetName;
    
    PresetDescriptor* presetDesc = getPresetByName(presetName.c_str());
    if(!presetDesc)
        throw primo::program_options::ParseFailure<char>("", presetName, "Parse error");
    
    preset = *presetDesc;
    return in;
}