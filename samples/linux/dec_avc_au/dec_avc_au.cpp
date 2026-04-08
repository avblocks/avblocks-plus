#include <primo/avblocks/avb++.h>

#include <print>
#include <iomanip>
#include <sstream>
#include <filesystem>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks::modern;
using namespace std;

namespace fs = std::filesystem;

static string buildOutputPath(Options& opt, int yuv_width, int yuv_height)
{
    if (!opt.output_file.empty())
        return opt.output_file;

    fs::path dir(getExeDir() + "/../../output/dec_avc_au");
    fs::create_directories(dir);

    ostringstream s;
    s << dir.c_str() << "/decoded_" << yuv_width << "x" << yuv_height << ".yuv";
    return s.str();
}

bool decode(Options& opt)
{
    try {
        // Use MediaInfo to detect stream parameters from the first AU file
        ostringstream s;
        s << opt.input_dir << "/au_" << setw(4) << setfill('0') << 0 << ".h264";

        TMediaInfo mediaInfo;
        mediaInfo.inputs(0).file(s.str());
        mediaInfo.open();

        // Resolve output frame dimensions from detected stream info, or override from options
        auto vsi = mediaInfo.outputs(0).pins(0).videoStreamInfo();
        int yuv_width  = opt.frame_size.width  > 0 ? opt.frame_size.width  : vsi.frameWidth();
        int yuv_height = opt.frame_size.height > 0 ? opt.frame_size.height : vsi.frameHeight();

        string outputFile = buildOutputPath(opt, yuv_width, yuv_height);
        deleteFile(outputFile.c_str());

        // Build output video stream info
        TVideoStreamInfo outVsi;
        outVsi
            .streamType(StreamType::UncompressedVideo)
            .colorFormat(opt.output_color.Id == ColorFormat::Unknown
                             ? ColorFormat::YUV420
                             : opt.output_color.Id)
            .frameWidth(yuv_width)
            .frameHeight(yuv_height)
            .scanType(ScanType::Progressive);

        if (opt.fps > 0)
            outVsi.frameRate(opt.fps);

        // Create push-mode transcoder
        TTranscoder transcoder;
        transcoder
            .allowDemoMode(true)
            .addInput(TMediaSocket(mediaInfo))
            .addOutput(
                TMediaSocket()
                    .file(outputFile)
                    .streamType(StreamType::UncompressedVideo)
                    .addPin(TMediaPin().streamInfo(outVsi))
            )
            .open();

        // Push AU files one by one
        for (int i = 0; ; ++i)
        {
            ostringstream auPath;
            auPath << opt.input_dir << "/au_" << setw(4) << setfill('0') << i << ".h264";

            vector<uint8_t> auData = readFileBytes(auPath.str().c_str());
            if (auData.empty())
                break;

            TMediaSample sample;
            sample.buffer(TMediaBuffer().attach(auData.data(), auData.size()));

            if (!transcoder.push(0, sample))
            {
                printError("Transcoder push", transcoder.error());
                transcoder.close();
                return false;
            }
        }

        if (!transcoder.flush())
        {
            printError("Transcoder flush", transcoder.error());
            transcoder.close();
            return false;
        }

        transcoder.close();

        println("Output: {}", outputFile);
        return true;

    } catch (const TAVBlocksException& ex) {
        println(stderr, "AVBlocks error: {}", ex.what());
        return false;
    } catch (const exception& ex) {
        println(stderr, "Error: {}", ex.what());
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
    return decode(opt) ? 0 : 1;
}
