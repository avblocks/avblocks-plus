#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    try {
        deleteFile(opt.h264_file.c_str());

        TTranscoderW()
            .allowDemoMode(true)
            .addInput(
                TMediaSocketW()
                    .file(opt.yuv_file)
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::UncompressedVideo)
                                .frameWidth(opt.frame_size.width)
                                .frameHeight(opt.frame_size.height)
                                .colorFormat(opt.yuv_color.Id)
                                .frameRate(opt.fps)
                                .scanType(ScanType::Progressive)
                            )
                    )
            )
            .addOutput(
                TMediaSocketW()
                    .file(opt.h264_file)
                    .streamType(StreamType::H264)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::H264)
                                .streamSubType(StreamSubType::AVC_Annex_B)
                            )
                    )
            )
            .open()
            .run()
            .close();

        std::wcout << L"Output: " << opt.h264_file << std::endl;
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
