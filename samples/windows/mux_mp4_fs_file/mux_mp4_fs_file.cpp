#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool mux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());

    try {
        TTranscoderW transcoder;

        // Transcoder demo mode must be enabled,
        // in order to use the production release for testing (without a valid license)
        transcoder.allowDemoMode(true);

        // Use the first H.264 video stream and the first AAC audio stream found in the inputs.
        // All other streams are disabled.
        bool videoDetected = false;
        bool audioDetected = false;

        for (const auto& file : opt.input_files)
        {
            auto input = TMediaSocketW(TMediaInfoW().file(file).open());

            for (int32_t i = 0; i < input.pins().count(); ++i)
            {
                auto pin = input.pins(i);
                const auto streamType = pin.streamInfo().streamType();

                if (streamType == StreamType::H264 && !videoDetected)
                {
                    videoDetected = true;
                    std::wcout << L"Muxing video input: " << file << std::endl;
                }
                else if (streamType == StreamType::AAC && !audioDetected)
                {
                    audioDetected = true;
                    std::wcout << L"Muxing audio input: " << file << std::endl;
                }
                else
                {
                    pin.connection(PinConnection::Disabled);
                }
            }

            transcoder.addInput(input);
        }

        if (!videoDetected && !audioDetected)
        {
            std::wcerr << L"No H264 video or AAC audio streams found in the input files." << std::endl;
            return false;
        }

        // Configure output
        auto output = TMediaSocketW()
            .file(opt.output_file)
            .streamType(StreamType::MP4);

        if (videoDetected)
        {
            output.addPin(TMediaPin()
                .streamInfo(TVideoStreamInfo()
                    .streamType(StreamType::H264)
                    .streamSubType(StreamSubType::AVC1)));
        }

        if (audioDetected)
        {
            output.addPin(TMediaPin()
                .streamInfo(TAudioStreamInfo()
                    .streamType(StreamType::AAC)
                    .streamSubType(StreamSubType::AAC_MP4)));
        }

        // Fast start: run a second pass that moves the moov atom before the mdat atom,
        // so that the file can be played while it is being downloaded.
        if (opt.fast_start)
        {
            output.addParam(Param::Muxer::MP4::FastStart, 1);
        }

        transcoder.addOutput(output).transcode();

        std::wcout << L"Output file: " << opt.output_file << std::endl;
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
    return mux(opt) ? 0 : 1;
}
