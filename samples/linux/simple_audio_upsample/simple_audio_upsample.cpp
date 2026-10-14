#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <print>

#include "../shim/shim23.h"

using namespace primo::avblocks::modern;

int main()
{
    TLibrary library;

    const char* inputFile  = "kahvi011_kennybeltrey-hydrate.mp3";
    const char* outputFile = "kahvi011_kennybeltrey-hydrate_48Khz.mp3";

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
                    .audioInfoRef(0, [](TAudioStreamInfo& audio) {
                        // Set output sampling rate to 48 KHz
                        audio.sampleRate(48000);
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
