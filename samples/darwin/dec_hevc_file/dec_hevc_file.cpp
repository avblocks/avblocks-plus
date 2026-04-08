#include <primo/avblocks/avb++.h>

#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool decode(const Options& opt)
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
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::UncompressedVideo)
                                .colorFormat(ColorFormat::YUV420)
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
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return decode(opt) ? 0 : 1;
}
