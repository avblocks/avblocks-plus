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
        MediaSocket inputSocket = MediaSocket()
                .file(opt.inputFile);
        
        // Create output socket with WAV format and LPCM audio
        MediaSocket outputSocket = MediaSocket()
                .file(opt.outputFile)
                .streamType(StreamType::WAVE)
                .addPin(MediaPin().audioStreamType(StreamType::LPCM));
        
        // Create and run transcoder
        Transcoder()
            .allowDemoMode(true)
            .addInput(inputSocket)
            .addOutput(outputSocket)
            .open()
            .run()
            .close();
        
        return true;
        
    } catch (const AVBlocksException& ex) {
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
        case Error: return 1;
        case Parsed: break;
    }

    Library library;
    return decode(opt) ? 0 : 1;
}