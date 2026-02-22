#include <primo/avblocks/avb++.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::avblocks::modern;
using namespace std;

// -----------------------------------------------------------------------
// NAL unit parsing (H.264/AVC spec)
// -----------------------------------------------------------------------

enum NALUType
{
    UNSPEC   = 0,
    SLICE    = 1,  // Coded slice of a non-IDR picture
    DPA      = 2,  // Coded slice data partition A
    DPB      = 3,  // Coded slice data partition B
    DPC      = 4,  // Coded slice data partition C
    IDR      = 5,  // Coded slice of an IDR picture
    SEI      = 6,  // Supplemental enhancement information
    SPS      = 7,  // Sequence parameter set
    PPS      = 8,  // Picture parameter set
    AUD      = 9,  // Access unit delimiter
    EOSEQ    = 10, // End of sequence
    EOSTREAM = 11, // End of stream
    FILL     = 12  // Filler data
};

enum NALUPriority
{
    DISPOSABLE = 0,
    LOW        = 1,
    HIGH       = 2,
    HIGHEST    = 3,
};

static NALUType     nal_unit_type    (uint8_t b) { return static_cast<NALUType>    (b & 0x1f); }
static NALUPriority nal_unit_ref_idc (uint8_t b) { return static_cast<NALUPriority>((b & 0x60) >> 5); }

#define MAP_ENUM_VALUE(p) strings[p] = #p

static ostream& operator<<(ostream& out, NALUType value)
{
    static map<NALUType, string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(SLICE); MAP_ENUM_VALUE(DPA);    MAP_ENUM_VALUE(DPB);
        MAP_ENUM_VALUE(DPC);   MAP_ENUM_VALUE(IDR);    MAP_ENUM_VALUE(SEI);
        MAP_ENUM_VALUE(SPS);   MAP_ENUM_VALUE(PPS);    MAP_ENUM_VALUE(AUD);
        MAP_ENUM_VALUE(EOSEQ); MAP_ENUM_VALUE(EOSTREAM); MAP_ENUM_VALUE(FILL);
    }
    return out << strings[value];
}

static ostream& operator<<(ostream& out, NALUPriority value)
{
    static map<NALUPriority, string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(DISPOSABLE); MAP_ENUM_VALUE(LOW);
        MAP_ENUM_VALUE(HIGH);       MAP_ENUM_VALUE(HIGHEST);
    }
    return out << strings[value];
}

static void print_nalu_header(const uint8_t* data)
{
    cout << left << "  " << setw(8) << nal_unit_type(*data) << ": "
         << nal_unit_ref_idc(*data) << "\n";
}

static void print_nalus(TMediaBuffer& buffer)
{
    while (buffer.dataSize() > 1)
    {
        int32_t        dataOffset = buffer.dataOffset();
        int32_t        dataSize   = buffer.dataSize();
        const uint8_t* data       = buffer.data();

        if (dataSize >= 3 && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01)
        {
            print_nalu_header(data + 3);
            buffer.setData(dataOffset + 3, dataSize - 3);
        }
        else if (dataSize >= 4 && data[0] == 0x00 && data[1] == 0x00 &&
                 data[2] == 0x00 && data[3] == 0x01)
        {
            print_nalu_header(data + 4);
            buffer.setData(dataOffset + 4, dataSize - 4);
        }
        else
        {
            buffer.setData(dataOffset + 1, dataSize - 1);
        }
    }
}

static void write_au_file(const string& outputDir, int au_index, TMediaBuffer& buffer)
{
    ostringstream name;
    name << outputDir << "/au_" << setw(4) << setfill('0') << au_index << ".h264";
    ofstream file(name.str(), ios::binary | ios::trunc);
    if (file)
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.dataSize());
}

// -----------------------------------------------------------------------

bool parse_h264_stream(const string& inputFile, const string& outputDir)
{
    try {
        TTranscoder transcoder;
        transcoder
            .addInput(
                TMediaSocket()
                    .file(inputFile)
            )
            .addOutput(
                // Pull-mode output: no file, just a video pin to receive AUs
                TMediaSocket()
                    .addPin(TMediaPin().streamInfo(TVideoStreamInfo()))
            )
            .open();

        if (!makeDir(outputDir))
        {
            cerr << "Cannot create output directory: " << outputDir << endl;
            return false;
        }

        int32_t outputIndex = 0;
        TMediaSample accessUnit;

        int au_index = 0;
        while (transcoder.pull(outputIndex, accessUnit))
        {
            auto buf = accessUnit.buffer();
            cout << "AU #" << au_index << ", " << buf.dataSize() << " bytes\n";
            write_au_file(outputDir, au_index, buf);
            print_nalus(buf);
            ++au_index;
        }

        const auto error = transcoder.error();
        if (!(error.facility() == primo::error::ErrorFacility::Codec &&
              error.code()     == primo::codecs::CodecError::EOS))
            printError("Transcoder pull", error);

        transcoder.close();
        return true;

    } catch (const TAVBlocksException& ex) {
        cerr << "AVBlocks error: " << ex.what() << endl;
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
    return parse_h264_stream(opt.input_file, opt.output_dir) ? 0 : 1;
}
