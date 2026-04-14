#include <primo/avblocks/avb++.h>
#include <numeric>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool cropVideo(const Options& opt)
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

        // Compute cropped output dimensions and update the output pin.
        // The resize filter uses frameWidth/frameHeight/displayRatio from the output pin.
        auto outVideoPin  = outSocket.pins().at(0);
        auto outVideoInfo = outVideoPin.videoStreamInfo();
        int32_t outWidth  = outVideoInfo.frameWidth()  - opt.cropLeft - opt.cropRight;
        int32_t outHeight = outVideoInfo.frameHeight() - opt.cropTop  - opt.cropBottom;
        int32_t g = std::gcd(outWidth, outHeight);
        outVideoInfo
            .frameWidth(outWidth)
            .frameHeight(outHeight)
            .displayRatioWidth(outWidth  / g)
            .displayRatioHeight(outHeight / g);

        // Add crop params — dimension mismatch already triggers decode→crop→encode.
        outVideoPin
            .addParam(Param::Video::Crop::Left,   opt.cropLeft)
            .addParam(Param::Video::Crop::Right,  opt.cropRight)
            .addParam(Param::Video::Crop::Top,    opt.cropTop)
            .addParam(Param::Video::Crop::Bottom, opt.cropBottom);

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
    return cropVideo(opt) ? 0 : 1;
}
