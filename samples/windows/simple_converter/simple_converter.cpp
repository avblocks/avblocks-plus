#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <iostream>

using namespace primo::codecs;
using namespace primo::avblocks::modern;

int wmain()
{
    TLibrary library;

    const wchar_t* inputFile  = L"Wildlife_h264_aac.mp4";
    const wchar_t* outputFile = L"Wildlife_h265_aac.mp4";

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
                        // Change the output video stream to H.265/HEVC
                        // Input is H.264/AVC at 700 kbps
                        // With H.265/HEVC we can use lower bitrate, e.g. 500 kbps
                        video.streamType(StreamType::H265)
                             .streamSubType(StreamSubType::HEVC_Annex_B)
                             .bitrate(500'000);
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
