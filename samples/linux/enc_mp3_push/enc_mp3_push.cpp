#include <primo/avblocks/avb++.h>

#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    deleteFile(opt.outputFile.c_str());

    try {
        // WAV reader: reads PCM from WAV file
        TTranscoder wavReader;
        wavReader
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
                    .file(opt.inputFile)
            )
            .addOutput(
                TMediaSocket()
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
        TTranscoder encoder;
        encoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
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
                TMediaSocket()
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
                    printError("Encoder push", encoder.error());
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
                    printError("WAV reader pull", error);
                    wavReader.close();
                    encoder.close();
                    return false;
                }
            }
        }

        wavReader.close();
        encoder.close();

        std::println("Output: {}", opt.outputFile);
        return true;

    } catch (const TAVBlocksException& ex) {
        std::println(stderr, "AVBlocks error: {}", ex.what());
        return false;
    } catch (const std::exception& ex) {
        std::println(stderr, "Error: {}", ex.what());
        return false;
    }
}

int main(int argc, char* argv[])
{
    Options opt;
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
        case Parsed: break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
