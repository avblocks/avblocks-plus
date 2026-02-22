#include <primo/avblocks/avb++.h>

#include <iostream>

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

        std::cout << "Output: " << opt.outputFile << std::endl;
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
    return decode(opt) ? 0 : 1;
}
