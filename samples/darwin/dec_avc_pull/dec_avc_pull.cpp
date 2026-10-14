#include <primo/avblocks/avb++.h>

#include <fstream>
#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool decodeAvcPull(const Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    std::ofstream outfile(opt.outputFile, std::ios_base::binary);
    if (!outfile.is_open())
    {
        std::println(stderr, "Could not open file {}", opt.outputFile);
        return false;
    }

    try {
        TTranscoder transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
                    .file(opt.inputFile)
            )
            .addOutput(
                // no file - decoded frames are pulled manually
                TMediaSocket()
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::UncompressedVideo)
                                .colorFormat(ColorFormat::YUV420)
                                .scanType(ScanType::Progressive)
                            )
                    )
            )
            .open();

        // decode by pulling decoded samples
        int32_t outputIndex = 0;
        int32_t frameIndex = 0;
        TMediaSample yuvFrame;

        while (transcoder.pull(outputIndex, yuvFrame))
        {
            // each call to Transcoder::pull returns a raw YUV 4:2:0 frame
            auto buffer = yuvFrame.buffer();
            outfile.write(reinterpret_cast<const char*>(buffer.data()), buffer.dataSize());

            std::println("Frame Index: {}", frameIndex);
            ++frameIndex;
        }

        const auto error = transcoder.error();
        const bool success = error.facility() == primo::error::ErrorFacility::Codec &&
                             error.code() == primo::codecs::CodecError::EOS;
        if (!success)
            printError("Transcoder pull", error);

        transcoder.close();

        std::println("Output: {}", opt.outputFile);
        std::println("Total frames: {}", frameIndex);

        return success;

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
    return decodeAvcPull(opt) ? 0 : 1;
}
