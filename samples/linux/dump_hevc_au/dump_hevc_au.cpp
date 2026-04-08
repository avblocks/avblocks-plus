#include <primo/avblocks/avb++.h>

#include <iostream>
#include <print>
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
// NAL unit parsing (H.265/HEVC spec, ITU-T H.265 Table 7-1)
// -----------------------------------------------------------------------

/*
 HEVC NAL unit header (2 bytes):
   forbidden_zero_bit    (1 bit)
   nal_unit_type         (6 bits)
   nuh_layer_id          (6 bits)
   nuh_temporal_id_plus1 (3 bits)

 nal_unit_type = (first_byte >> 1) & 0x3F
*/

enum NALUType
{
    TRAIL_N    = 0,   // Trailing picture, non-reference
    TRAIL_R    = 1,   // Trailing picture, reference
    TSA_N      = 2,   // Temporal sub-layer access, non-reference
    TSA_R      = 3,   // Temporal sub-layer access, reference
    STSA_N     = 4,   // Step-wise temporal sub-layer access, non-reference
    STSA_R     = 5,   // Step-wise temporal sub-layer access, reference
    RADL_N     = 6,   // Random access decodable leading, non-reference
    RADL_R     = 7,   // Random access decodable leading, reference
    RASL_N     = 8,   // Random access skipped leading, non-reference
    RASL_R     = 9,   // Random access skipped leading, reference
    BLA_W_LP   = 16,  // Broken link access with leading pictures
    BLA_W_RADL = 17,  // Broken link access with RADL pictures
    BLA_N_LP   = 18,  // Broken link access without leading pictures
    IDR_W_RADL = 19,  // IDR with RADL pictures
    IDR_N_LP   = 20,  // IDR without leading pictures
    CRA_NUT    = 21,  // Clean random access
    VPS_NUT    = 32,  // Video parameter set
    SPS_NUT    = 33,  // Sequence parameter set
    PPS_NUT    = 34,  // Picture parameter set
    AUD_NUT    = 35,  // Access unit delimiter
    EOS_NUT    = 36,  // End of sequence
    EOB_NUT    = 37,  // End of bitstream
    FD_NUT     = 38,  // Filler data
    PREFIX_SEI = 39,  // Supplemental enhancement information (prefix)
    SUFFIX_SEI = 40,  // Supplemental enhancement information (suffix)
};

static NALUType nal_unit_type(uint8_t first_byte)
{
    // nal_unit_type occupies bits [6:1] of the first header byte
    return static_cast<NALUType>((first_byte >> 1) & 0x3F);
}

#define MAP_ENUM_VALUE(p) strings[p] = #p

static ostream& operator<<(ostream& out, NALUType value)
{
    static map<NALUType, string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(TRAIL_N);    MAP_ENUM_VALUE(TRAIL_R);
        MAP_ENUM_VALUE(TSA_N);      MAP_ENUM_VALUE(TSA_R);
        MAP_ENUM_VALUE(STSA_N);     MAP_ENUM_VALUE(STSA_R);
        MAP_ENUM_VALUE(RADL_N);     MAP_ENUM_VALUE(RADL_R);
        MAP_ENUM_VALUE(RASL_N);     MAP_ENUM_VALUE(RASL_R);
        MAP_ENUM_VALUE(BLA_W_LP);   MAP_ENUM_VALUE(BLA_W_RADL);
        MAP_ENUM_VALUE(BLA_N_LP);   MAP_ENUM_VALUE(IDR_W_RADL);
        MAP_ENUM_VALUE(IDR_N_LP);   MAP_ENUM_VALUE(CRA_NUT);
        MAP_ENUM_VALUE(VPS_NUT);    MAP_ENUM_VALUE(SPS_NUT);
        MAP_ENUM_VALUE(PPS_NUT);    MAP_ENUM_VALUE(AUD_NUT);
        MAP_ENUM_VALUE(EOS_NUT);    MAP_ENUM_VALUE(EOB_NUT);
        MAP_ENUM_VALUE(FD_NUT);     MAP_ENUM_VALUE(PREFIX_SEI);
        MAP_ENUM_VALUE(SUFFIX_SEI);
    }
    auto it = strings.find(value);
    if (it != strings.end())
        return out << it->second;
    return out << "RSV_" << static_cast<int>(value);
}

static void print_nalu_header(const uint8_t* data)
{
    // HEVC NAL unit header is 2 bytes; nal_unit_type is in the first byte
    cout << left << "  " << setw(12) << nal_unit_type(*data) << "\n";
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
    name << outputDir << "/au_" << setw(4) << setfill('0') << au_index << ".h265";
    ofstream file(name.str(), ios::binary | ios::trunc);
    if (file)
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.dataSize());
}

// -----------------------------------------------------------------------

bool parse_h265_stream(const string& inputFile, const string& outputDir)
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
                    .addPin(TMediaPin().streamInfo(
                        TVideoStreamInfo().streamType(StreamType::H265)))
            )
            .open();

        if (!makeDir(outputDir))
        {
            println(stderr, "Cannot create output directory: {}", outputDir);
            return false;
        }

        int32_t outputIndex = 0;
        TMediaSample accessUnit;

        int au_index = 0;
        while (transcoder.pull(outputIndex, accessUnit))
        {
            auto buf = accessUnit.buffer();
            println("AU #{}, {} bytes", au_index, buf.dataSize());
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
        println(stderr, "AVBlocks error: {}", ex.what());
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
    return parse_h265_stream(opt.input_file, opt.output_dir) ? 0 : 1;
}
