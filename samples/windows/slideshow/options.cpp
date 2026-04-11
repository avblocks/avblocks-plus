#include "stdafx.h"

#include "options.h"
#include "program_options.h"
#include "util.h"

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
    { Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,    "ts" },
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
    wcout << L"\nPRESETS" << endl;
    wcout << L"-------" << endl;
    for (int i = 0; i < avb_presets_len; ++i)
    {
        const PresetDescriptor& preset = avb_presets[i];
        wcout << left << setw(45) << toWide(preset.name) << L" ." << toWide(preset.extension) << endl;
    }
}

PresetDescriptor* getPresetByName(const wchar_t* presetName)
{
    for (int i = 0; i < avb_presets_len; ++i)
    {
        PresetDescriptor* preset = &avb_presets[i];
        if (0 == _wcsicmp(toWide(preset->name).c_str(), presetName))
            return preset;
    }
    return NULL;
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"\nUsage: slideshow --input <directory> --output <file> [--preset <PRESET>]";
    wcout << L" [--presets]";
    wcout << endl;
    primo::program_options::doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir = getExeDir() + L"/../../assets/img";

    std::filesystem::path output(getExeDir() + L"/../../output/slideshow");
    std::filesystem::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/cube.mp4";
    opt.output_file = s.str();

    opt.preset = *getPresetByName(toWide(Preset::Video::Generic::MP4::Base_H264_AAC).c_str());
}

bool validateOptions(Options& opt)
{
    if (!opt.preset.name)
    {
        opt.preset = *getPresetByName(toWide(Preset::Video::Generic::MP4::Base_H264_AAC).c_str());
    }

    // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
    {
        wstring::size_type pos = opt.output_file.rfind(L'.');
        if (0 == pos || wstring::npos == pos)
        {
            opt.output_file.append(L".");
        }
        else
        {
            opt.output_file = opt.output_file.substr(0, pos + 1);
        }

        opt.output_file.append(toWide(opt.preset.extension));
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
        wcout << L" --input " << opt.input_dir;
        wcout << L" --output " << opt.output_file;
        wcout << L" --preset " << toWide(opt.preset.name);
        wcout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,         L"")
        (L"input,i",  opt.input_dir,    wstring(), L"input directory containing images for the slideshow.")
        (L"output,o", opt.output_file,  wstring(), L"output filename (without extension). The extension is added based on the preset.")
        (L"preset,p", opt.preset,       PresetDescriptor(), L"output preset id. Use --presets to list presets.")
        (L"presets",  opt.list_presets, L"list presets");

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

std::wistringstream& operator>>(std::wistringstream& in, PresetDescriptor& preset)
{
    std::wstring wname;
    in >> wname;

    PresetDescriptor* presetDesc = getPresetByName(wname.c_str());
    if (!presetDesc)
        throw primo::program_options::ParseFailure<wchar_t>(L"", wname, L"Parse error");

    preset = *presetDesc;
    return in;
}
