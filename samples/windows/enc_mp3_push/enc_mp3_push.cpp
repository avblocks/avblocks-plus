#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());

    try {
        // WAV reader: reads PCM from WAV file
        TTranscoderW wavReader;
        wavReader
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .file(opt.inputFile)
            )
            .addOutput(
                TMediaSocketW()
                    .streamType(StreamType::LPCM)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::LPCM)
                                .channels(2)
                                .sampleRate(48000)
                                .bitsPerSample(16)
                            )
                    )
            )
            .open();

        // Encoder: receives pushed PCM, writes MP3 to file
        TTranscoderW encoder;
        encoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .streamType(StreamType::LPCM)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TAudioStreamInfo()
                                .streamType(StreamType::LPCM)
                                .channels(2)
                                .sampleRate(48000)
                                .bitsPerSample(16)
                            )
                    )
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
                            )
                    )
            )
            .open();

        // Push loop: pull PCM from wavReader, push to encoder
        int32_t wavOutputIndex = 0;
        TMediaSample sample;

        for (;;)
        {
            if (wavReader.pull(wavOutputIndex, sample))
            {
                if (!encoder.push(0, sample))
                {
                    printError(L"Encoder push", encoder.error());
                    wavReader.close();
                    encoder.close();
                    return false;
                }
            }
            else
            {
                auto error = wavReader.error();
                if (error.facility() == primo::error::ErrorFacility::Codec &&
                    error.code()     == primo::codecs::CodecError::EOS)
                {
                    encoder.pushEos(0);
                    break;
                }
                else
                {
                    printError(L"WAV reader pull", error);
                    wavReader.close();
                    encoder.close();
                    return false;
                }
            }
        }

        wavReader.close();
        encoder.close();

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
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
