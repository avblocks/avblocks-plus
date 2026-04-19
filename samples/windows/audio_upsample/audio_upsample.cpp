#include "stdafx.h"

#include <primo/avblocks/avb++.h>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool upsample(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());
    try {
        TTranscoderW()
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .file(opt.inputFile)
            )
            .addOutput(
                TMediaSocketW()
                    .file(opt.outputFile)
                    .streamType(StreamType::MPEG_Audio)
                    .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::MPEG_Audio)
                                .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                                .sampleRate(48000)  // Resample to 48 KHz
                            )
                    )
            )
            .open()
            .run()
            .close();

        std::wcout << L"Output: " << opt.outputFile << std::endl;
        return true;

    } catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << ex.what() << std::endl;
        return false;
    } catch (const std::exception& ex) {
        std::wcerr << L"Error: " << ex.what() << std::endl;
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    TLibrary library;
    return upsample(opt) ? 0 : 1;
}
