#include "stdafx.h"

/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool reEncode(const Options& opt)
{
    std::wcout << L"Input file: " << opt.inputFile << std::endl;
    std::wcout << L"Output file: " << opt.outputFile << std::endl;
    std::wcout << L"Re-encode audio forced: " << (opt.reEncodeAudio.val ? L"yes" : L"no") << std::endl;
    std::wcout << L"Re-encode video forced: " << (opt.reEncodeVideo.val ? L"yes" : L"no") << std::endl;

    deleteFile(opt.outputFile.c_str());

    try {
        // Probe input
        TMediaInfoW mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        TTranscoderW transcoder;
        transcoder.allowDemoMode(true);

        // Add input from probe
        transcoder.addInput(TMediaSocketW(mediaInfo));

        // Build output socket — same container type as input
        TMediaSocketW outputSocket;
        outputSocket
            .streamType(transcoder.inputs(0).streamType())
            .file(opt.outputFile);

        // Mirror all input pins to output, optionally forcing re-encode
        auto inSocket = transcoder.inputs(0);
        for (int i = 0; i < inSocket.pins().count(); ++i)
        {
            auto inPin = inSocket.pins(i);
            auto si    = inPin.streamInfo();

            TMediaPin outPin;

            if (si.mediaType() == MediaType::Video)
            {
                outPin.streamInfo(inPin.videoStreamInfo());
                if (opt.reEncodeVideo.val)
                    outPin.addParam(Param::ReEncode, Use::On);
            }
            else if (si.mediaType() == MediaType::Audio)
            {
                outPin.streamInfo(inPin.audioStreamInfo());
                if (opt.reEncodeAudio.val)
                    outPin.addParam(Param::ReEncode, Use::On);
            }

            outputSocket.addPin(std::move(outPin));
        }

        transcoder.addOutput(outputSocket);
        transcoder.open().run().close();

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
    return reEncode(opt) ? 0 : 1;
}
