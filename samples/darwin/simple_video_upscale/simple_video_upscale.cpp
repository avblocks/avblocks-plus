#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <print>

#include "../shim/shim23.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

int main()
{
    TLibrary library;

    const char* inputFile  = "AndrewConnolly_2014.mp4";
    const char* outputFile = "AndrewConnolly_2014_1080p.mp4";

    // transcoder will fail if output exists (by design)
    std::remove(outputFile);

    try {
        // Probe the input file and create the input socket from the probe
        auto input = TMediaSocket(TMediaInfo().file(inputFile).open());

        // Demo mode is allowed when using the demo version of the library.
        TTranscoder()
            .allowDemoMode(true)
            .addInput(input)
            // The output starts as a copy of the input
            .addOutput(
                input.clone()
                    .file(outputFile)
                    .videoInfoRef(0, [](TVideoStreamInfo& video) {
                        // Set the new frame width and height to Full HD 1920x1080
                        video.frameWidth(1920).frameHeight(1080);
                    })
                    .pinRef(0, [](TMediaPin& pin) {
                        // Set the resize interpolation method:
                        // InterpolationMethod::Cubic is best for upscaling (however, it is slow)
                        pin.addParam(Param::Video::Resize::InterpolationMethod, InterpolationMethod::Cubic);
                    })
            )
            .transcode();

        std::println("Output: {}", outputFile);
        return 0;

    } catch (const TAVBlocksException& ex) {
        std::println(stderr, "AVBlocks error: {}", ex.what());
        return 1;
    }
}
