#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <iostream>

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

int wmain()
{
    TLibrary library;

    const wchar_t* inputFile  = L"AndrewConnolly_2014.mp4";
    const wchar_t* outputFile = L"AndrewConnolly_2014_1080p.mp4";

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

        std::wcout << L"Output: " << outputFile << std::endl;
        return 0;

    } catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << ex.what() << std::endl;
        return 1;
    }
}
