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
    const char* outputFile = "AndrewConnolly_2014_640x480_Padded.mp4";

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
                        // Set the new frame width and height to 640 x 480 and the display ratio to 4:3
                        video.frameWidth(640).frameHeight(480)
                             .displayRatioWidth(4).displayRatioHeight(3);
                    })
                    .pinRef(0, [](TMediaPin& pin) {
                        // The input video is 16:9, 854 x 480,
                        // to squeeze it in 640 x 480 and keep the 16:9 display ratio,
                        // the height has to be: 640 * 9 / 16
                        //
                        // Pad by (480 - 640 * 9 / 16) / 2 pixels on each side.
                        //
                        // The padding will trigger downscaling, so we also set the interpolation method.
                        // For best result when downscaling, use InterpolationMethod::Super
                        pin.addParam(Param::Video::Pad::Top,    (480 - 640 * 9 / 16) / 2)
                           .addParam(Param::Video::Pad::Bottom, (480 - 640 * 9 / 16) / 2)
                           .addParam(Param::Video::Resize::InterpolationMethod, InterpolationMethod::Super);
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
