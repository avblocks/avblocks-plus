#include <primo/avblocks/avb++.h>

#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool decode(const Options& opt)
{
    try {
        // Delete output file if it exists
        deleteFile(opt.outputFile.c_str());
        
        // Create input socket from file.
        TMediaSocket inputSocket = TMediaSocket().file(opt.inputFile);
        
        // Create output socket with WAV format and LPCM audio
        TMediaSocket outputSocket = TMediaSocket()
            .file(opt.outputFile)
            .streamType(StreamType::WAVE)
            .addPin(TMediaPin()
                .streamInfo(TAudioStreamInfo()
                    .streamType(StreamType::LPCM)));
        
        // Create and run transcoder
        TTranscoder()
            .allowDemoMode(true)
            .addInput(inputSocket)
            .addOutput(outputSocket)
            .open()
            .run()
            .close();
        
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
    return decode(opt) ? 0 : 1;
}