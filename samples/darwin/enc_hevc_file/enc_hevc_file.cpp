#include <primo/avblocks/avb++.h>

#include <iostream>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    try {
        deleteFile(opt.h265_file.c_str());

        TTranscoder()
            .allowDemoMode(true)
            .addInput(
                TMediaSocket()
                    .file(opt.yuv_file)
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::UncompressedVideo)
                                .frameWidth(opt.frame_size.width_)
                                .frameHeight(opt.frame_size.height_)
                                .colorFormat(opt.yuv_color.Id)
                                .frameRate(opt.fps)
                                .scanType(ScanType::Progressive)
                            )
                    )
            )
            .addOutput(
                TMediaSocket()
                    .file(opt.h265_file)
                    .streamType(StreamType::H265)
                    .addPin(
                        TMediaPin()
                            .streamInfo(TVideoStreamInfo()
                                .streamType(StreamType::H265)
                                .streamSubType(StreamSubType::HEVC_Annex_B)
                            )
                    )
            )
            .open()
            .run()
            .close();

        std::cout << "Output: " << opt.h265_file << std::endl;
        return true;

    } catch (const TAVBlocksException& ex) {
        std::cerr << "AVBlocks error: " << ex.what() << std::endl;
        return false;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[])
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
