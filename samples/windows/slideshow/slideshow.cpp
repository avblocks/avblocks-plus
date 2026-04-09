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

static primo::avblocks::modern::TMediaBuffer loadImageFile(const wchar_t* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::wcerr << L"Cannot open image file: " << filename << std::endl;
        return primo::avblocks::modern::TMediaBuffer(0);
    }

    auto imgsize = static_cast<int32_t>(file.tellg());
    file.seekg(0);

    primo::avblocks::modern::TMediaBuffer buffer(imgsize);
    file.read(reinterpret_cast<char*>(buffer.start()), imgsize);
    buffer.setData(0, static_cast<int32_t>(file.gcount()));

    return buffer;
}

bool slideshow(const Options& opt)
{
    std::wcout << L"preset: " << toWide(opt.preset.name) << std::endl;

    const double inputFramerate = 25.0;
    const int imageCount = 250;

    deleteFile(opt.output_file.c_str());

    try {
        // Probe the first image to get frame dimensions
        TMediaInfoW mediaInfo;
        std::wstring firstImage = opt.input_dir + L"/cube0000.jpeg";
        mediaInfo.inputs(0).file(firstImage);
        mediaInfo.open();

        // Build input socket: no file — we will push JPEG frames manually
        auto vsi = mediaInfo.outputs(0).pins(0).videoStreamInfo().clone();
        vsi.frameRate(inputFramerate);

        TTranscoderW transcoder;
        transcoder.allowDemoMode(true);
        transcoder.addInput(
            TMediaSocketW()
                .addPin(TMediaPin().streamInfo(vsi))
        );

        // Output via preset
        transcoder.addOutput(
            TMediaSocketW(opt.preset.name)
                .file(opt.output_file)
        );

        transcoder.open();

        TMediaSample sample;
        for (int i = 0; i < imageCount; ++i)
        {
            // Build image path: input_dir + "/cube" + 4-digit index + ".jpeg"
            std::wostringstream pathStream;
            pathStream << opt.input_dir << L"/cube";
            pathStream << std::setfill(L'0') << std::setw(4) << i;
            pathStream << L".jpeg";
            std::wstring imagePath = pathStream.str();

            TMediaBuffer buffer = loadImageFile(imagePath.c_str());
            sample.buffer(std::move(buffer));
            sample.startTime(i / inputFramerate);

            if (!transcoder.push(0, sample))
            {
                printError(L"Push Transcoder", transcoder.error());
                transcoder.close();
                return false;
            }
        }

        if (!transcoder.flush())
        {
            printError(L"Flush Transcoder", transcoder.error());
            transcoder.close();
            return false;
        }

        transcoder.close();
        std::wcout << L"Output video: \"" << opt.output_file << L"\"" << std::endl;
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
    return slideshow(opt) ? 0 : 1;
}
