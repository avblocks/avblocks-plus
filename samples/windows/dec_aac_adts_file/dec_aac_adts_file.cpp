#include "stdafx.h"

#include <primo/avblocks/avb++.h>

#include <iostream>

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
        TMediaSocketW inputSocket = TMediaSocketW()
                .file(opt.inputFile);
        
        // Create output socket with WAV format and LPCM audio
        TMediaSocketW outputSocket = TMediaSocketW()
                .file(opt.outputFile)
                .streamType(StreamType::WAVE)
                .addPin(TMediaPin()
                    .streamInfo(TAudioStreamInfo()
                        .streamType(StreamType::LPCM)));
        
        // Create and run transcoder
        TTranscoderW()
            .allowDemoMode(true)
            .addInput(inputSocket)
            .addOutput(outputSocket)
            .open()
            .run()
            .close();
        
        return true;
        
    } catch (const TAVBlocksException& ex) {
        std::cerr << "AVBlocks error: " << ex.what() << std::endl;
        return false;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
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