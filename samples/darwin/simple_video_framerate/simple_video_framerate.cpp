#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <print>

#include "../shim/shim23.h"

using namespace primo::avblocks::modern;

int main()
{
    TLibrary library;

    const char* inputFile  = "AndrewConnolly_2014.mp4";
    const char* outputFile = "AndrewConnolly_2014_30fps.mp4";

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
                        // Change the frame rate from 24 fps to 30 fps
                        video.frameRate(30.0);
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
