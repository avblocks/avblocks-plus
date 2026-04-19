#include "stdafx.h"

#include <primo/avblocks/avb++.h>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool upscaleVideo(const Options& opt)
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

        // Set the new frame size and request cubic interpolation (best for upscaling)
        auto outVideoPin = outSocket.pins().at(0);
        outVideoPin.videoStreamInfo()
            .frameWidth(opt.width)
            .frameHeight(opt.height);

        outVideoPin.addParam(
            Param::Video::Resize::InterpolationMethod, 
            int64_t(InterpolationMethod::Cubic));

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
    return upscaleVideo(opt) ? 0 : 1;
}
