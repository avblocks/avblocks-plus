#include <primo/avblocks/avb++.h>

#include <fstream>
#include <print>
#include <string>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;

static TMediaBuffer loadImageFile(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::println(stderr, "Cannot open image file: {}", filename);
        return TMediaBuffer(0);
    }

    auto imgsize = static_cast<int32_t>(file.tellg());
    file.seekg(0);

    TMediaBuffer buffer(imgsize);
    file.read(reinterpret_cast<char*>(buffer.start()), imgsize);
    buffer.setData(0, static_cast<int32_t>(file.gcount()));

    return buffer;
}

bool slideshow(const Options& opt)
{
    std::println("preset: {}", opt.preset.name);

    const double inputFramerate = 25.0;
    const int imageCount = 250;

    deleteFile(opt.output_file.c_str());

    try {
        // Probe the first image to get frame dimensions
        TMediaInfo mediaInfo;
        std::string firstImage = std::string(opt.input_dir) + "/cube0000.jpeg";
        mediaInfo.inputs(0).file(firstImage);
        mediaInfo.open();

        // Build input socket: no file — we will push JPEG frames manually
        auto vsi = mediaInfo.outputs(0).pins(0).videoStreamInfo().clone();
        vsi.frameRate(inputFramerate);

        TTranscoder transcoder;
        transcoder.allowDemoMode(true);
        transcoder.addInput(
            TMediaSocket()
                .addPin(TMediaPin().streamInfo(vsi))
        );

        // Output via preset
        transcoder.addOutput(
            TMediaSocket(opt.preset.name)
                .file(opt.output_file)
        );

        transcoder.open();

        TMediaSample sample;
        for (int i = 0; i < imageCount; ++i)
        {
            auto imagePath = std::format("{}/cube{:04d}.jpeg", opt.input_dir, i);

            TMediaBuffer buffer = loadImageFile(imagePath.c_str());
            sample.buffer(std::move(buffer));
            sample.startTime(i / inputFramerate);

            if (!transcoder.push(0, sample))
            {
                printError("Push Transcoder", transcoder.error());
                transcoder.close();
                return false;
            }
        }

        if (!transcoder.flush())
        {
            printError("Flush Transcoder", transcoder.error());
            transcoder.close();
            return false;
        }

        transcoder.close();
        std::println("Output video: \"{}\"", opt.output_file);
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
    return slideshow(opt) ? 0 : 1;
}
