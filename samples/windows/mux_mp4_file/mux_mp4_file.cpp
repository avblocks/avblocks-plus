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

bool mux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());

    try {
        TTranscoderW transcoder;
        transcoder.allowDemoMode(true);

        TMediaSocketW outputSocket;
        outputSocket.file(opt.output_file).streamType(StreamType::MP4);

        // audio inputs
        for (const auto& audioFile : opt.input_audio)
        {
            transcoder.addInput(
                TMediaSocketW()
                    .file(audioFile)
                    .streamType(StreamType::MP4)
            );
            outputSocket.addPin(TMediaPin()
                .streamInfo(TAudioStreamInfo()
                    .streamType(StreamType::AAC)));

            std::wcout << L"Muxing audio input: " << audioFile << std::endl;
        }

        // video inputs
        for (const auto& videoFile : opt.input_video)
        {
            transcoder.addInput(
                TMediaSocketW()
                    .file(videoFile)
                    .streamType(StreamType::MP4)
            );
            outputSocket.addPin(TMediaPin()
                .streamInfo(TVideoStreamInfo()
                    .streamType(StreamType::H264)));

            std::wcout << L"Muxing video input: " << videoFile << std::endl;
        }

        transcoder.addOutput(outputSocket);
        transcoder.open().run().close();

        std::wcout << L"Output file: " << opt.output_file << std::endl;
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
    return mux(opt) ? 0 : 1;
}
