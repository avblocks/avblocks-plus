#include <primo/avblocks/avb++.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::codecs;
using namespace primo::avblocks::modern;

// ---- Enum-to-string helpers -------------------------------------------------

static const char* colorFormatName(ColorFormat::Enum color)
{
    switch (color)
    {
        case ColorFormat::GRAY:     return "GRAY";
        case ColorFormat::Unknown:  return "Unknown";
        case ColorFormat::NV12:     return "NV12";
        case ColorFormat::BGR24:    return "BGR24";
        case ColorFormat::BGR32:    return "BGR32";
        case ColorFormat::BGRA32:   return "BGRA32";
        case ColorFormat::BGR444:   return "BGR444";
        case ColorFormat::BGR555:   return "BGR555";
        case ColorFormat::BGR565:   return "BGR565";
        case ColorFormat::UYVY:     return "UYVY";
        case ColorFormat::Y411:     return "Y411";
        case ColorFormat::Y41P:     return "Y41P";
        case ColorFormat::YUV411:   return "YUV411";
        case ColorFormat::YUV420:   return "YUV420";
        case ColorFormat::YUV420A:  return "YUV420A";
        case ColorFormat::YUV422:   return "YUV422";
        case ColorFormat::YUV422A:  return "YUV422A";
        case ColorFormat::YUV444:   return "YUV444";
        case ColorFormat::YUV444A:  return "YUV444A";
        case ColorFormat::YUY2:     return "YUY2";
        case ColorFormat::YV12:     return "YV12";
        case ColorFormat::YVU9:     return "YVU9";
        default:                    return "???";
    }
}

static const char* bitrateModeName(BitrateMode::Enum mode)
{
    switch (mode)
    {
        case BitrateMode::Unknown:  return "Unknown";
        case BitrateMode::ABR:      return "ABR";
        case BitrateMode::CBR:      return "CBR";
        case BitrateMode::VBR:      return "VBR";
        default:                    return "???";
    }
}

static const char* scanTypeName(ScanType::Enum scan)
{
    switch (scan)
    {
        case ScanType::Progressive:       return "Progressive";
        case ScanType::BottomFieldFirst:  return "BottomFieldFirst";
        case ScanType::TopFieldFirst:     return "TopFieldFirst";
        case ScanType::Unknown:           return "Unknown";
        default:                          return "???";
    }
}

static const char* streamTypeName(StreamType::Enum st)
{
    switch (st)
    {
        // video
        case StreamType::H261:              return "H261";
        case StreamType::H263:              return "H263";
        case StreamType::H264:              return "H264";
        case StreamType::H265:              return "H265";
        case StreamType::MJPEG:             return "MJPEG";
        case StreamType::MPEG1_Video:       return "MPEG1_Video";
        case StreamType::MPEG2_Video:       return "MPEG2_Video";
        case StreamType::MPEG4_Video:       return "MPEG4_Video";
        case StreamType::UncompressedVideo: return "UncompressedVideo";
        case StreamType::VC1:               return "VC1";
        case StreamType::WMV:               return "WMV";
        case StreamType::AVS:               return "AVS";
        case StreamType::VP8:               return "VP8";
        case StreamType::VP9:               return "VP9";
        case StreamType::AV1:               return "AV1";
        case StreamType::Theora:            return "Theora";
        // audio
        case StreamType::LPCM:              return "LPCM";
        case StreamType::ALAW_PCM:          return "ALAW_PCM";
        case StreamType::MULAW_PCM:         return "MULAW_PCM";
        case StreamType::G726_ADPCM:        return "G726_ADPCM";
        case StreamType::VIDEO_DVD_PCM:     return "VIDEO_DVD_PCM";
        case StreamType::AC3:               return "AC3";
        case StreamType::DTS:               return "DTS";
        case StreamType::MPEG_Audio:        return "MPEG_Audio";
        case StreamType::Vorbis:            return "Vorbis";
        case StreamType::Opus:              return "Opus";
        case StreamType::AAC:               return "AAC";
        case StreamType::AMRNB:             return "AMRNB";
        case StreamType::AMRWB:             return "AMRWB";
        case StreamType::WMA:               return "WMA";
        // images
        case StreamType::BMP:               return "BMP";
        case StreamType::GIF:               return "GIF";
        case StreamType::JPEG:              return "JPEG";
        case StreamType::PNG:               return "PNG";
        case StreamType::TIFF:              return "TIFF";
        // containers
        case StreamType::AVI:               return "AVI";
        case StreamType::MP4:               return "MP4";
        case StreamType::ASF:               return "ASF";
        case StreamType::MPEG_PS:           return "MPEG_PS";
        case StreamType::MPEG_TS:           return "MPEG_TS";
        case StreamType::MPEG_PES:          return "MPEG_PES";
        case StreamType::WAVE:              return "WAVE";
        case StreamType::FLV:               return "FLV";
        case StreamType::WebM:              return "WebM";
        case StreamType::OGG:               return "Ogg";
        case StreamType::IVF:               return "IVF";
        // data
        case StreamType::MPEG_PSI_PACKETS:  return "MPEG_PSI_PACKETS";
        case StreamType::MPEG_TS_PACKETS:   return "MPEG_TS_PACKETS";
        case StreamType::Teletext:          return "Teletext";
        case StreamType::Unknown:           return "Unknown";
        default:                            return "???";
    }
}

static const char* streamSubTypeName(StreamSubType::Enum sst)
{
    switch (sst)
    {
        case StreamSubType::Unknown:                 return "Unknown";
        case StreamSubType::None:                    return "None";
        case StreamSubType::AVC1:                    return "AVC1";
        case StreamSubType::AVC_Annex_B:             return "AVC_Annex_B";
        case StreamSubType::HVC1:                    return "HVC1";
        case StreamSubType::HEVC_Annex_B:            return "HEVC_Annex_B";
        case StreamSubType::AAC_RAW:                 return "AAC_RAW";
        case StreamSubType::AAC_ADTS:                return "AAC_ADTS";
        case StreamSubType::AAC_ADIF:                return "AAC_ADIF";
        case StreamSubType::AAC_MP4:                 return "AAC_MP4";
        case StreamSubType::MPEG_TS_BDAV:            return "MPEG_TS_BDAV";
        case StreamSubType::MPEG_Audio_Layer1:       return "MPEG_Audio_Layer1";
        case StreamSubType::MPEG_Audio_Layer2:       return "MPEG_Audio_Layer2";
        case StreamSubType::MPEG_Audio_Layer3:       return "MPEG_Audio_Layer3";
        case StreamSubType::G726_PACKED_AAL2:        return "G726_PACKED_AAL2";
        case StreamSubType::G726_PACKED_RFC3551:     return "G726_PACKED_RFC3551";
        case StreamSubType::MPEG1_System:            return "MPEG1_System";
        case StreamSubType::MPEG2_System:            return "MPEG2_System";
        default:                                     return "???";
    }
}

static const char* mediaTypeName(MediaType::Enum mt)
{
    switch (mt)
    {
        case MediaType::Unknown: return "Unknown";
        case MediaType::Audio:   return "Audio";
        case MediaType::Video:   return "Video";
        case MediaType::Text:    return "Text";
        case MediaType::Data:    return "Data";
        default:                 return "???";
    }
}

// ---- Print helpers ----------------------------------------------------------

static void printVideo(TVideoStreamInfo& vsi)
{
    cout << "frame size: "     << vsi.frameWidth() << "x" << vsi.frameHeight() << endl;
    cout << "display ratio: "  << vsi.displayRatioWidth() << ":" << vsi.displayRatioHeight() << endl;
    cout << "frame rate: "     << vsi.frameRate() << endl;
    cout << "color format: "   << colorFormatName(vsi.colorFormat()) << endl;
    cout << "bitrate: "        << vsi.bitrate()
         << ", mode: "         << bitrateModeName(static_cast<BitrateMode::Enum>(vsi.bitrateMode())) << endl;
    cout << "scan type: "      << scanTypeName(vsi.scanType()) << endl;
    cout << "frame bottom up: "<< vsi.frameBottomUp() << endl;
}

static void printAudio(TAudioStreamInfo& asi)
{
    cout << "sample rate: "    << asi.sampleRate() << endl;
    cout << "channels: "       << asi.channels() << endl;
    cout << "bits per sample: "<< asi.bitsPerSample() << endl;
    cout << "bytes per frame: "<< asi.bytesPerFrame() << endl;
    cout << "bitrate: "        << asi.bitrate()
         << ", mode: "         << bitrateModeName(static_cast<BitrateMode::Enum>(asi.bitrateMode())) << endl;
    cout << "channel layout: " << asi.channelLayout() << endl;
    cout << "flags: "          << asi.pcmFlags() << endl;
}

static void printStreams(TMediaInfo& mi)
{
    cout << "file: " << mi.inputs(0).file() << endl;

    for (int32_t s = 0; s < mi.outputs().count(); ++s)
    {
        auto socket = mi.outputs(s);
        cout << "container: " << streamTypeName(socket.streamType()) << endl;

        auto pins = socket.pins();
        int32_t count = pins.count();
        cout << "streams: " << count << endl;
        cout << endl;

        for (int32_t i = 0; i < count; ++i)
        {
            auto pin = pins.at(i);
            auto si  = pin.streamInfo();

            cout << "stream #" << i << " " << mediaTypeName(si.mediaType()) << endl;
            cout << "type: "    << streamTypeName(si.streamType());
            cout << ", subtype: " << streamSubTypeName(si.streamSubType()) << endl;
            cout << "id: "      << si.ID() << endl;
            cout << "duration: "<< si.duration() << endl;

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

            cout << endl;
        }
    }
}

// ---- Entry points -----------------------------------------------------------

bool avInfo(Options& opt)
{
    TMediaInfo mi;
    mi.inputs(0).file(opt.inputFile);

    if (mi.tryOpen())
    {
        printStreams(mi);
        return true;
    }
    else
    {
        printError("MediaInfo open", mi.error());
        return false;
    }
}

int main(int argc, char* argv[])
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
