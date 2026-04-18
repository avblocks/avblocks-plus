#include <primo/avblocks/avb++.h>

#include <iostream>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool changeVideoFrameRate(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());
    try {
        TMediaInfo mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        // Create input socket from probe (preserves file path + detected pins)
        TMediaSocket inSocket(mediaInfo);

        // Create output socket from same probe (copies detected pins), then set output file
        TMediaSocket outSocket(mediaInfo);
        outSocket.file(opt.outputFile);

        // Change the frame rate on the output video pin
        auto outVideoPin  = outSocket.pins().at(0);
        outVideoPin.videoStreamInfo().frameRate(opt.frameRate);

        TTranscoder transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(inSocket)
            .addOutput(outSocket)
            .open()
            .run()
            .close();

        std::println("Output: {}", opt.outputFile);
        return true;
    } catch (const TAVBlocksException& ex) {
        std::println(stderr, "AVBlocks error: {}", ex.what());
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
    return changeVideoFrameRate(opt) ? 0 : 1;
}
