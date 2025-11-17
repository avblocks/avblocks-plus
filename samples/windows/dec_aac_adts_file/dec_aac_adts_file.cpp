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
        _MediaSocketW inputSocket = _MediaSocketW()
                .file(opt.inputFile);
        
        // Create output socket with WAV format and LPCM audio
        _MediaSocketW outputSocket = _MediaSocketW()
                .file(opt.outputFile)
                .streamType(StreamType::WAVE)
                .addPin(_MediaPin().audioStreamType(StreamType::LPCM));
        
        // Create and run transcoder
        _TranscoderW()
            .allowDemoMode(true)
            .addInput(inputSocket)
            .addOutput(outputSocket)
            .open()
            .run()
            .close();
        
        return true;
        
    } catch (const _AVBlocksException& ex) {
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

    _Library library;
    return decode(opt) ? 0 : 1;
}