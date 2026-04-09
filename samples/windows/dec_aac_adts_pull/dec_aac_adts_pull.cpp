#include "stdafx.h"

#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;
using namespace std;


bool decode(Options &opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    try {
        // Create decoder transcoder - input from file, output PCM without file
        TTranscoderW decoder;
        decoder.allowDemoMode(true)
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

        // Create WAV writer transcoder - input PCM without file, output to file
        TTranscoderW wavWriter;
        wavWriter.allowDemoMode(true)
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
                    .streamType(StreamType::WAVE)
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

        // Pull-push decoding loop
        int32_t decoderOutputIndex = 0;
        TMediaSample pcmSample;

        bool decoderEos = false;
        while (!decoderEos) {
            // Pull PCM sample from decoder
            if (decoder.pull(decoderOutputIndex, pcmSample)) {
                // Push PCM sample to WAV writer
                if (!wavWriter.push(0, pcmSample)) {
                    printError(L"WAV Writer push", wavWriter.error());
                    return false;
                }
                continue;
            }

            // No more PCM data from decoder
            const auto error = decoder.error();
            if (error.facility() == primo::error::ErrorFacility::Codec &&
                error.code()     == primo::codecs::CodecError::EOS) {
                // Signal EOS to WAV writer
                wavWriter.pushEos(0);
                decoderEos = true;
                continue;
            }

            printError(L"Decoder pull", error);
            return false;
        }

        decoder.close();
        wavWriter.close();

        return true;
    }
    catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << ex.what() << std::endl;
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
    case Command:
        return 0;
    case Error:
        return 1;
    case Parsed:
        break;
    }

    TLibrary library;
    bool result = decode(opt);
    return result ? 0 : 1;
}
