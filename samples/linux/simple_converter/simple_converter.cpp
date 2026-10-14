#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <print>

#include "../shim/shim23.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

int main()
{
    TLibrary library;

    const char* inputFile  = "Wildlife_h264_aac.mp4";
    const char* outputFile = "Wildlife_h265_aac.mp4";

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
                        // Change the output video stream to H.265/HEVC
                        // Input is H.264/AVC at 700 kbps
                        // With H.265/HEVC we can use lower bitrate, e.g. 500 kbps
                        video.streamType(StreamType::H265)
                             .streamSubType(StreamSubType::HEVC_Annex_B)
                             .bitrate(500'000);
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
