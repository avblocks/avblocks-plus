#include <primo/avblocks/avb++.h>

#include <print>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    try {
        deleteFile(opt.output_file.c_str());

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
                                .frameWidth(opt.yuv_frame.width)
                                .frameHeight(opt.yuv_frame.height)
                                .colorFormat(opt.yuv_color.Id)
                                .frameRate(opt.yuv_fps)
                                .scanType(ScanType::Progressive)
                            )
                    )
            )
            .addOutput(
                // Construct from preset name — pre-configures all codec settings
                TMediaSocket(opt.preset.name)
                    .file(opt.output_file)
            )
            .open()
            .run()
            .close();

        std::println("Output: {}", opt.output_file);
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
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return encode(opt) ? 0 : 1;
}
