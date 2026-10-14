#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <iostream>

using namespace primo::avblocks::modern;

int wmain()
{
    TLibrary library;

    const wchar_t* inputFile  = L"AndrewConnolly_2014.mp4";
    const wchar_t* outputFile = L"AndrewConnolly_2014_30fps.mp4";

    // transcoder will fail if output exists (by design)
    _wremove(outputFile);

    try {
        // Probe the input file and create the input socket from the probe
        auto input = TMediaSocketW(TMediaInfoW().file(inputFile).open());

        // Demo mode is allowed when using the demo version of the library.
        TTranscoderW()
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

        std::wcout << L"Output: " << outputFile << std::endl;
        return 0;

    } catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << ex.what() << std::endl;
        return 1;
    }
}
