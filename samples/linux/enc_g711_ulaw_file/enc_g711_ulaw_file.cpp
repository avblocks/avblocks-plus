#include <primo/avblocks/avb++.h>

#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
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
                    .streamType(StreamType::WAVE)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::MULAW_PCM)
                                .sampleRate(8000)
                                .channels(1)
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
    return encode(opt) ? 0 : 1;
}
