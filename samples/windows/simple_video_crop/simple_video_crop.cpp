#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <iostream>

using namespace primo::avblocks;
using namespace primo::avblocks::modern;

int wmain()
{
    TLibrary library;

    const wchar_t* inputFile  = L"AndrewConnolly_2014.mp4";
    const wchar_t* outputFile = L"AndrewConnolly_2014_cropped.mp4";

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
                        // Set the new frame width and height to 640 x 480 and the display ratio to 4:3
                        video.frameWidth(640).frameHeight(480)
                             .displayRatioWidth(4).displayRatioHeight(3);
                    })
                    .pinRef(0, [](TMediaPin& pin) {
                        // The input video is 854x480.
                        // To make it 640x480 we have to cut (854 - 640) / 2 pixels from each side.
                        pin.addParam(Param::Video::Crop::Left,  (854 - 640) / 2)
                           .addParam(Param::Video::Crop::Right, (854 - 640) / 2);
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
