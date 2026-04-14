#include <primo/avblocks/avb++.h>

#include <string>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool upsample(const Options& opt)
{
    try {
        deleteFile(opt.outputFile.c_str());

        TTranscoder()
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
                    .file(opt.inputFile)
            )
            .addOutput(
                TMediaSocket()
                    .file(opt.outputFile)
                    .streamType(StreamType::MPEG_Audio)
                    .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::MPEG_Audio)
                                .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                                .sampleRate(48000)  // Resample to 48 KHz
                            )
                    )
            )
            .open()
            .run()
            .close();

        std::println("Output: {}", opt.outputFile);
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
        case Error: return 1;
        case Parsed: break;
    }

    TLibrary library;
    return upsample(opt) ? 0 : 1;
}
