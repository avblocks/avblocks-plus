#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

static TVideoStreamInfo createYuvVideoInfo(const Options& opt)
{
    return TVideoStreamInfo()
        .streamType(StreamType::UncompressedVideo)
        .frameRate(opt.fps)
        .frameWidth(opt.frame_size.width)
        .frameHeight(opt.frame_size.height)
        .colorFormat(opt.yuv_color.Id)
        .scanType(ScanType::Progressive);
}

bool encode(const Options& opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(opt.h264_file.c_str());

    try {
        // YUV reader: simulates a raw video source (e.g. a capture device).
        // The output socket has no file, so frames are pulled one by one.
        TTranscoderW yuvReader;
        yuvReader
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .file(opt.yuv_file)
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(TMediaPin().streamInfo(createYuvVideoInfo(opt)))
            )
            .addOutput(
                TMediaSocketW()
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(TMediaPin().streamInfo(createYuvVideoInfo(opt)))
            )
            .open();

        // H.264 encoder: receives pushed YUV frames, writes AVC/H.264 Annex B to file.
        TTranscoderW encoder;
        encoder
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(TMediaPin().streamInfo(createYuvVideoInfo(opt)))
            )
            .addOutput(
                // default subtype for StreamType::H264 is StreamSubType::AVC_Annex_B
                TMediaSocketW()
                    .file(opt.h264_file)
                    .streamType(StreamType::H264)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::H264)
                                .frameRate(opt.fps)
                                .frameWidth(opt.frame_size.width)
                                .frameHeight(opt.frame_size.height)
                                .colorFormat(opt.yuv_color.Id)
                                .scanType(ScanType::Progressive)
                            )
                            // enable or disable hardware acceleration
                            .addParam(Param::HardwareEncoder, HardwareEncoder::Auto)
                    )
            )
            .open();

        // Push loop: pull a raw frame from yuvReader, push it to the encoder
        int32_t yuvOutputIndex = 0;
        TMediaSample yuvFrame;
        bool success = false;

        for (;;)
        {
            if (yuvReader.pull(yuvOutputIndex, yuvFrame))
            {
                if (!encoder.push(0, yuvFrame))
                {
                    printError(L"H264 encoder push", encoder.error());
                    break;
                }
                continue;
            }

            const auto error = yuvReader.error();
            if (error.facility() != primo::error::ErrorFacility::Codec ||
                error.code() != CodecError::EOS)
            {
                printError(L"YUV reader pull", error);
                break;
            }

            // end of input - flush the encoder
            if (!encoder.flush())
            {
                printError(L"H264 encoder flush", encoder.error());
                break;
            }

            success = true;
            break;
        }

        yuvReader.close();
        encoder.close();

        if (success)
            std::wcout << L"Output: " << opt.h264_file << std::endl;

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
