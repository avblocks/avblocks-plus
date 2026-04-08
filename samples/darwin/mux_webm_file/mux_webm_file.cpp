#include <primo/avblocks/avb++.h>

#include <print>
#include <string>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

bool mux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());
    
    try {
        TTranscoder transcoder;
        transcoder.allowDemoMode(true);

        TMediaSocket outputSocket;
        outputSocket.file(opt.output_file).streamType(StreamType::WebM);

        // audio inputs
        for (const auto& audioFile : opt.input_audio)
        {
            transcoder.addInput(
                TMediaSocket()
                    .file(audioFile)
                    .streamType(StreamType::WebM)
            );
            outputSocket.addPin(TMediaPin()
                .streamInfo(TAudioStreamInfo()
                    .streamType(StreamType::Vorbis)));

            std::println("Muxing audio input: {}", audioFile);
        }

        // video inputs
        for (const auto& videoFile : opt.input_video)
        {
            transcoder.addInput(
                TMediaSocket()
                    .file(videoFile)
                    .streamType(StreamType::WebM)
            );
            outputSocket.addPin(TMediaPin()
                .streamInfo(TVideoStreamInfo()
                    .streamType(StreamType::VP8)));

            std::println("Muxing video input: {}", videoFile);
        }

        transcoder.addOutput(outputSocket);
        transcoder.open().run().close();

        std::println("Output file: {}", opt.output_file);
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
    return mux(opt) ? 0 : 1;
}
