#include <primo/avblocks/avb++.h>

#include <iostream>
#include <fstream>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.h264_file.c_str());

    std::ofstream outfile(opt.h264_file, std::ios_base::binary);
    if (!outfile.is_open())
    {
        std::cerr << "Could not open output file: " << opt.h264_file << std::endl;
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
                    .streamType(StreamType::H264)
                    .streamSubType(StreamSubType::AVC_Annex_B)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::H264)
                                .streamSubType(StreamSubType::AVC_Annex_B)
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

        std::cout << "Output: " << opt.h264_file << std::endl;
        return true;

    } catch (const TAVBlocksException& ex) {
        std::cerr << "AVBlocks error: " << ex.what() << std::endl;
        return false;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
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
