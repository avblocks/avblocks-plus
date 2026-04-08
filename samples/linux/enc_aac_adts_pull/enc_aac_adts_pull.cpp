#include <primo/avblocks/avb++.h>

#include <print>
#include <fstream>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());

    std::ofstream outfile(opt.outputFile, std::ios_base::binary);
    if (!outfile.is_open())
    {
        std::println(stderr, "Could not open file: {}", opt.outputFile);
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
                // No file on the output socket — we pull samples manually
                TMediaSocket()
                    .streamType(StreamType::AAC)
                    .streamSubType(StreamSubType::AAC_ADTS)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::AAC)
                                .streamSubType(StreamSubType::AAC_ADTS)
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
        bool success = (error.facility() == primo::error::ErrorFacility::Codec &&
                        error.code()     == primo::codecs::CodecError::EOS);

        if (!success)
            printError("Transcoder pull", error);

        transcoder.close();

        if (success)
            std::println("Output: {}", opt.outputFile);

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
        case Error: return 1;
        case Parsed: break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
