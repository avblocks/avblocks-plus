#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::codecs;
using namespace primo::avblocks::modern;

// ---- Enum-to-string helpers -------------------------------------------------

static const wchar_t* colorFormatName(ColorFormat::Enum color)
{
    switch (color)
    {
        case ColorFormat::GRAY:     return L"GRAY";
        case ColorFormat::Unknown:  return L"Unknown";
        case ColorFormat::NV12:     return L"NV12";
        case ColorFormat::BGR24:    return L"BGR24";
        case ColorFormat::BGR32:    return L"BGR32";
        case ColorFormat::BGRA32:   return L"BGRA32";
        case ColorFormat::BGR444:   return L"BGR444";
        case ColorFormat::BGR555:   return L"BGR555";
        case ColorFormat::BGR565:   return L"BGR565";
        case ColorFormat::UYVY:     return L"UYVY";
        case ColorFormat::Y411:     return L"Y411";
        case ColorFormat::Y41P:     return L"Y41P";
        case ColorFormat::YUV411:   return L"YUV411";
        case ColorFormat::YUV420:   return L"YUV420";
        case ColorFormat::YUV420A:  return L"YUV420A";
        case ColorFormat::YUV422:   return L"YUV422";
        case ColorFormat::YUV422A:  return L"YUV422A";
        case ColorFormat::YUV444:   return L"YUV444";
        case ColorFormat::YUV444A:  return L"YUV444A";
        case ColorFormat::YUY2:     return L"YUY2";
        case ColorFormat::YV12:     return L"YV12";
        case ColorFormat::YVU9:     return L"YVU9";
        default:                    return L"???";
    }
}

static const wchar_t* bitrateModeName(BitrateMode::Enum mode)
{
    switch (mode)
    {
        case BitrateMode::Unknown:  return L"Unknown";
        case BitrateMode::ABR:      return L"ABR";
        case BitrateMode::CBR:      return L"CBR";
        case BitrateMode::VBR:      return L"VBR";
        default:                    return L"???";
    }
}

static const wchar_t* scanTypeName(ScanType::Enum scan)
{
    switch (scan)
    {
        case ScanType::Progressive:       return L"Progressive";
        case ScanType::BottomFieldFirst:  return L"BottomFieldFirst";
        case ScanType::TopFieldFirst:     return L"TopFieldFirst";
        case ScanType::Unknown:           return L"Unknown";
        default:                          return L"???";
    }
}

static const wchar_t* streamTypeName(StreamType::Enum st)
{
    switch (st)
    {
        // video
        case StreamType::H261:              return L"H261";
        case StreamType::H263:              return L"H263";
        case StreamType::H264:              return L"H264";
        case StreamType::H265:              return L"H265";
        case StreamType::MJPEG:             return L"MJPEG";
        case StreamType::MPEG1_Video:       return L"MPEG1_Video";
        case StreamType::MPEG2_Video:       return L"MPEG2_Video";
        case StreamType::MPEG4_Video:       return L"MPEG4_Video";
        case StreamType::UncompressedVideo: return L"UncompressedVideo";
        case StreamType::VC1:               return L"VC1";
        case StreamType::WMV:               return L"WMV";
        case StreamType::AVS:               return L"AVS";
        case StreamType::VP8:               return L"VP8";
        case StreamType::VP9:               return L"VP9";
        case StreamType::AV1:               return L"AV1";
        case StreamType::Theora:            return L"Theora";
        // audio
        case StreamType::LPCM:              return L"LPCM";
        case StreamType::ALAW_PCM:          return L"ALAW_PCM";
        case StreamType::MULAW_PCM:         return L"MULAW_PCM";
        case StreamType::G726_ADPCM:        return L"G726_ADPCM";
        case StreamType::VIDEO_DVD_PCM:     return L"VIDEO_DVD_PCM";
        case StreamType::AC3:               return L"AC3";
        case StreamType::DTS:               return L"DTS";
        case StreamType::MPEG_Audio:        return L"MPEG_Audio";
        case StreamType::Vorbis:            return L"Vorbis";
        case StreamType::Opus:              return L"Opus";
        case StreamType::AAC:               return L"AAC";
        case StreamType::AMRNB:             return L"AMRNB";
        case StreamType::AMRWB:             return L"AMRWB";
        case StreamType::WMA:               return L"WMA";
        // images
        case StreamType::BMP:               return L"BMP";
        case StreamType::GIF:               return L"GIF";
        case StreamType::JPEG:              return L"JPEG";
        case StreamType::PNG:               return L"PNG";
        case StreamType::TIFF:              return L"TIFF";
        // containers
        case StreamType::AVI:               return L"AVI";
        case StreamType::MP4:               return L"MP4";
        case StreamType::ASF:               return L"ASF";
        case StreamType::MPEG_PS:           return L"MPEG_PS";
        case StreamType::MPEG_TS:           return L"MPEG_TS";
        case StreamType::MPEG_PES:          return L"MPEG_PES";
        case StreamType::WAVE:              return L"WAVE";
        case StreamType::FLV:               return L"FLV";
        case StreamType::WebM:              return L"WebM";
        case StreamType::OGG:               return L"Ogg";
        case StreamType::IVF:               return L"IVF";
        // data
        case StreamType::MPEG_PSI_PACKETS:  return L"MPEG_PSI_PACKETS";
        case StreamType::MPEG_TS_PACKETS:   return L"MPEG_TS_PACKETS";
        case StreamType::Teletext:          return L"Teletext";
        case StreamType::Unknown:           return L"Unknown";
        default:                            return L"???";
    }
}

static const wchar_t* streamSubTypeName(StreamSubType::Enum sst)
{
    switch (sst)
    {
        case StreamSubType::Unknown:                 return L"Unknown";
        case StreamSubType::None:                    return L"None";
        case StreamSubType::AVC1:                    return L"AVC1";
        case StreamSubType::AVC_Annex_B:             return L"AVC_Annex_B";
        case StreamSubType::HVC1:                    return L"HVC1";
        case StreamSubType::HEVC_Annex_B:            return L"HEVC_Annex_B";
        case StreamSubType::AAC_RAW:                 return L"AAC_RAW";
        case StreamSubType::AAC_ADTS:                return L"AAC_ADTS";
        case StreamSubType::AAC_ADIF:                return L"AAC_ADIF";
        case StreamSubType::AAC_MP4:                 return L"AAC_MP4";
        case StreamSubType::MPEG_TS_BDAV:            return L"MPEG_TS_BDAV";
        case StreamSubType::MPEG_Audio_Layer1:       return L"MPEG_Audio_Layer1";
        case StreamSubType::MPEG_Audio_Layer2:       return L"MPEG_Audio_Layer2";
        case StreamSubType::MPEG_Audio_Layer3:       return L"MPEG_Audio_Layer3";
        case StreamSubType::G726_PACKED_AAL2:        return L"G726_PACKED_AAL2";
        case StreamSubType::G726_PACKED_RFC3551:     return L"G726_PACKED_RFC3551";
        case StreamSubType::MPEG1_System:            return L"MPEG1_System";
        case StreamSubType::MPEG2_System:            return L"MPEG2_System";
        default:                                     return L"???";
    }
}

static const wchar_t* mediaTypeName(MediaType::Enum mt)
{
    switch (mt)
    {
        case MediaType::Unknown: return L"Unknown";
        case MediaType::Audio:   return L"Audio";
        case MediaType::Video:   return L"Video";
        case MediaType::Text:    return L"Text";
        case MediaType::Data:    return L"Data";
        default:                 return L"???";
    }
}

// ---- Print helpers ----------------------------------------------------------

static void printVideo(TVideoStreamInfo& vsi)
{
    wcout << L"frame size: "      << vsi.frameWidth() << L"x" << vsi.frameHeight() << endl;
    wcout << L"display ratio: "   << vsi.displayRatioWidth() << L":" << vsi.displayRatioHeight() << endl;
    wcout << L"frame rate: "      << vsi.frameRate() << endl;
    wcout << L"color format: "    << colorFormatName(vsi.colorFormat()) << endl;
    wcout << L"bitrate: "         << vsi.bitrate()
          << L", mode: "          << bitrateModeName(static_cast<BitrateMode::Enum>(vsi.bitrateMode())) << endl;
    wcout << L"scan type: "       << scanTypeName(vsi.scanType()) << endl;
    wcout << L"frame bottom up: " << vsi.frameBottomUp() << endl;
}

static void printAudio(TAudioStreamInfo& asi)
{
    wcout << L"sample rate: "     << asi.sampleRate() << endl;
    wcout << L"channels: "        << asi.channels() << endl;
    wcout << L"bits per sample: " << asi.bitsPerSample() << endl;
    wcout << L"bytes per frame: " << asi.bytesPerFrame() << endl;
    wcout << L"bitrate: "         << asi.bitrate()
          << L", mode: "          << bitrateModeName(static_cast<BitrateMode::Enum>(asi.bitrateMode())) << endl;
    wcout << L"channel layout: "  << asi.channelLayout() << endl;
    wcout << L"flags: "           << asi.pcmFlags() << endl;
}

static void printStreams(TMediaInfoW& mi)
{
    wcout << L"file: " << mi.inputs(0).file() << endl;

    for (int32_t s = 0; s < mi.outputs().count(); ++s)
    {
        auto socket = mi.outputs(s);
        wcout << L"container: " << streamTypeName(socket.streamType()) << endl;

        auto pins = socket.pins();
        int32_t count = pins.count();
        wcout << L"streams: " << count << endl;
        wcout << endl;

        for (int32_t i = 0; i < count; ++i)
        {
            auto pin = pins.at(i);
            auto si  = pin.streamInfo();

            wcout << L"stream #" << i << L" " << mediaTypeName(si.mediaType()) << endl;
            wcout << L"type: "    << streamTypeName(si.streamType());
            wcout << L", subtype: " << streamSubTypeName(si.streamSubType()) << endl;
            wcout << L"id: "      << si.ID() << endl;
            wcout << L"duration: "<< si.duration() << endl;

            if (si.mediaType() == MediaType::Video)
            {
                auto vsi = pin.videoStreamInfo();
                printVideo(vsi);
            }
            else if (si.mediaType() == MediaType::Audio)
            {
                auto asi = pin.audioStreamInfo();
                printAudio(asi);
            }

            wcout << endl;
        }
    }
}

bool avInfo(Options& opt)
{
    TMediaInfoW mi;
    mi.inputs(0).file(opt.inputFile);

    if (mi.tryOpen())
    {
        printStreams(mi);
        return true;
    }
    else
    {
        printError(L"MediaInfo open", mi.error());
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;
    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return avInfo(opt) ? 0 : 1;
}
