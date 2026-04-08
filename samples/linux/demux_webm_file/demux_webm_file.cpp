#include <primo/avblocks/avb++.h>

#include <print>
#include <string>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool demux(const Options& opt)
{
    try {
        TMediaInfo mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        TMediaSocket inSocket(mediaInfo);

        TTranscoder transcoder;
        transcoder.allowDemoMode(true);
        transcoder.addInput(inSocket);

        bool audio = false;
        bool video = false;

        for (int i = 0; i < inSocket.pins().count(); ++i)
        {
            auto pin = inSocket.pins(i);
            auto mediaType = pin.streamInfo().mediaType();

            std::string fileName;
            if (mediaType == MediaType::Audio && !audio)
            {
                audio = true;
                fileName = opt.outputFile + ".aud.webm";
            }
            else if (mediaType == MediaType::Video && !video)
            {
                video = true;
                fileName = opt.outputFile + ".vid.webm";
            }
            else
            {
                pin.connection(PinConnection::Disabled);
                continue;
            }

            std::println("Output file: {}", fileName);
            deleteFile(fileName.c_str());

            TMediaSocket outSocket;
            outSocket.addPin(TMediaPin(pin.get()));
            outSocket.file(fileName);

            transcoder.addOutput(outSocket);
        }

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
    return demux(opt) ? 0 : 1;
}

