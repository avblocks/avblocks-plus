#include <primo/avblocks/avb++.h>

#include <cstdio>
#include <iostream>

using namespace primo::avblocks::modern;

int wmain()
{
    TLibrary library;

    const wchar_t* inputFile  = L"kahvi011_kennybeltrey-hydrate.mp3";
    const wchar_t* outputFile = L"kahvi011_kennybeltrey-hydrate_48Khz.mp3";

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
                    .audioInfoRef(0, [](TAudioStreamInfo& audio) {
                        // Set output sampling rate to 48 KHz
                        audio.sampleRate(48000);
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
