#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());

    std::ofstream outfile(opt.outputFile, std::ios_base::binary);
    if (!outfile.is_open())
    {
        std::wcerr << L"Could not open file: " << opt.outputFile << std::endl;
        return false;
    }

    try {
        TTranscoderW transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .file(opt.inputFile)
            )
            .addOutput(
                // No file on the output socket -- we pull samples manually
                TMediaSocketW()
                    .streamType(StreamType::MPEG_Audio)
                    .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::MPEG_Audio)
                                .streamSubType(StreamSubType::MPEG_Audio_Layer3)
                            )
                    )
            )
            .open();

        int32_t outputIndex = 0;
        TMediaSample sample;

        while (transcoder.pull(outputIndex, sample))
        {
            auto buf = sample.buffer();
            outfile.write(reinterpret_cast<const char*>(buf.data()), buf.dataSize());
        }

        const auto error = transcoder.error();
        bool success = (error.facility() == primo::error::ErrorFacility::Codec &&
                        error.code()     == primo::codecs::CodecError::EOS);

        if (!success)
            printError(L"Transcoder pull", error);

        transcoder.close();

        if (success)
            std::wcout << L"Output: " << opt.outputFile << std::endl;

        return success;

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
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
