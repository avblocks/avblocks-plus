#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;

bool demux(const Options& opt)
{
    try {
        TMediaInfoW mediaInfo;
        mediaInfo.inputs(0).file(opt.inputFile);
        mediaInfo.open();

        TMediaSocketW inSocket(mediaInfo);

        TTranscoderW transcoder;
        transcoder.allowDemoMode(true);
        transcoder.addInput(inSocket);

        bool audio = false;
        bool video = false;

        for (int i = 0; i < inSocket.pins().count(); ++i)
        {
            auto pin = inSocket.pins(i);
            auto mediaType = pin.streamInfo().mediaType();

            std::wstring fileName;
            if (mediaType == MediaType::Audio && !audio)
            {
                audio = true;
                fileName = opt.outputFile + L".aud.mp4";
            }
            else if (mediaType == MediaType::Video && !video)
            {
                video = true;
                fileName = opt.outputFile + L".vid.mp4";
            }
            else
            {
                pin.connection(PinConnection::Disabled);
                continue;
            }

            std::wcout << L"Output file: " << fileName << std::endl;
            deleteFile(fileName.c_str());

            TMediaSocketW outSocket;
            outSocket.addPin(TMediaPin(pin.get()));
            outSocket.file(fileName);

            transcoder.addOutput(outSocket);
        }

        transcoder.open().run().close();
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
    return demux(opt) ? 0 : 1;
}
