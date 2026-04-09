#include "stdafx.h"

/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool encode(const Options& opt)
{
    try {
        deleteFile(opt.output_file.c_str());

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
                TMediaSocketW(opt.preset.name)
                    .file(opt.output_file)
            )
            .open()
            .run()
            .close();

        std::wcout << L"Output: " << opt.output_file << std::endl;
        return true;

    } catch (const TAVBlocksException& ex) {
        std::wcerr << L"AVBlocks error: " << toWide(ex.what()) << std::endl;
        return false;
    } catch (const std::exception& ex) {
        std::wcerr << L"Error: " << toWide(ex.what()) << std::endl;
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
