#include <primo/avblocks/avb++.h>

#include <print>
#include <string>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool reEncode(const Options& opt)
{
    std::println("Input file: {}", opt.inputFile);
    std::println("Output file: {}", opt.outputFile);
    std::println("Re-encode audio forced: {}", opt.reEncodeAudio.val ? "yes" : "no");
    std::println("Re-encode video forced: {}", opt.reEncodeVideo.val ? "yes" : "no");

    deleteFile(opt.outputFile.c_str());

    try {
        // Probe input
        TMediaInfo mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        TTranscoder transcoder;
        transcoder.allowDemoMode(true);

        // Add input from probe
        transcoder.addInput(TMediaSocket(mediaInfo));

        // Build output socket — same container type as input
        TMediaSocket outputSocket;
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
    return reEncode(opt) ? 0 : 1;
}
