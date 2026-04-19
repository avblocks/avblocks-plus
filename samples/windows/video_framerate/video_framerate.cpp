#include "stdafx.h"

#include <primo/avblocks/avb++.h>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool changeVideoFrameRate(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());
    try {
        TMediaInfoW mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        // Create input socket from probe (preserves file path + detected pins)
        TMediaSocketW inSocket(mediaInfo);

        // Create output socket from same probe (copies detected pins), then set output file
        TMediaSocketW outSocket(mediaInfo);
        outSocket.file(opt.outputFile);

        // Change the frame rate on the output video pin
        auto outVideoPin = outSocket.pins().at(0);
        outVideoPin.videoStreamInfo().frameRate(opt.frameRate);

        TTranscoderW transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(inSocket)
            .addOutput(outSocket)
            .open()
            .run()
            .close();

        std::wcout << L"Output: " << opt.outputFile << std::endl;
        return true;
    } catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << ex.what() << std::endl;
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
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
