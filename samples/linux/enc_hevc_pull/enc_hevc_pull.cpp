#include <primo/avblocks/avb++.h>

#include <print>
#include <fstream>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.h265_file.c_str());

    std::ofstream outfile(opt.h265_file, std::ios_base::binary);
    if (!outfile.is_open())
    {
        std::println(stderr, "Could not open output file: {}", opt.h265_file);
        return false;
    }

    try {
        TTranscoder transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
                    .file(opt.yuv_file)
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::UncompressedVideo)
                                .frameWidth(opt.frame_size.width_)
                                .frameHeight(opt.frame_size.height_)
                                .colorFormat(opt.yuv_color.Id)
                                .frameRate(opt.fps)
                                .scanType(ScanType::Progressive)
                            )
                    )
            )
            .addOutput(
                TMediaSocket()
                    .streamType(StreamType::H265)
                    .streamSubType(StreamSubType::HEVC_Annex_B)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::H265)
                                .streamSubType(StreamSubType::HEVC_Annex_B)
                            )
                    )
            )
            .open();

        int32_t outputIndex = 0;
        TMediaSample sample;

        while (transcoder.pull(outputIndex, sample))
        {
            auto buf = sample.buffer();
            outfile.write(reinterpret_cast<const char*>(buf.data()), buf.dataSize());
        }

        const auto error = transcoder.error();
        bool success = error.facility() == primo::error::ErrorFacility::Codec &&
                       error.code()     == primo::codecs::CodecError::EOS;

        if (!success)
        {
            printError("Transcoder pull", error);
            transcoder.close();
            return false;
        }

        transcoder.close();

        std::println("Output: {}", opt.h265_file);
        return true;

    } catch (const TAVBlocksException& ex) {
        std::println(stderr, "AVBlocks error: {}", ex.what());
        return false;
    } catch (const std::exception& ex) {
        std::println(stderr, "Error: {}", ex.what());
        return false;
    }
}

int main(int argc, char* argv[])
{
    Options opt;
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
