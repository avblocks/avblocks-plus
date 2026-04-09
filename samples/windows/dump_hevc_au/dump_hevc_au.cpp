#include "stdafx.h"

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

static wostream& operator<<(wostream& out, NALUType value)
{
    static map<NALUType, wstring> strings;
    if (strings.empty())
    {
        strings[TRAIL_N]    = L"TRAIL_N";    strings[TRAIL_R]    = L"TRAIL_R";
        strings[TSA_N]      = L"TSA_N";      strings[TSA_R]      = L"TSA_R";
        strings[STSA_N]     = L"STSA_N";     strings[STSA_R]     = L"STSA_R";
        strings[RADL_N]     = L"RADL_N";     strings[RADL_R]     = L"RADL_R";
        strings[RASL_N]     = L"RASL_N";     strings[RASL_R]     = L"RASL_R";
        strings[BLA_W_LP]   = L"BLA_W_LP";   strings[BLA_W_RADL] = L"BLA_W_RADL";
        strings[BLA_N_LP]   = L"BLA_N_LP";   strings[IDR_W_RADL] = L"IDR_W_RADL";
        strings[IDR_N_LP]   = L"IDR_N_LP";   strings[CRA_NUT]    = L"CRA_NUT";
        strings[VPS_NUT]    = L"VPS_NUT";    strings[SPS_NUT]    = L"SPS_NUT";
        strings[PPS_NUT]    = L"PPS_NUT";    strings[AUD_NUT]    = L"AUD_NUT";
        strings[EOS_NUT]    = L"EOS_NUT";    strings[EOB_NUT]    = L"EOB_NUT";
        strings[FD_NUT]     = L"FD_NUT";     strings[PREFIX_SEI] = L"PREFIX_SEI";
        strings[SUFFIX_SEI] = L"SUFFIX_SEI";
    }
    auto it = strings.find(value);
    if (it != strings.end())
        return out << it->second;
    return out << L"RSV_" << static_cast<int>(value);
}

static void print_nalu_header(const uint8_t* data)
{
    // HEVC NAL unit header is 2 bytes; nal_unit_type is in the first byte
    wcout << left << L"  " << setw(12) << nal_unit_type(*data) << L"\n";
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

static void write_au_file(const wstring& outputDir, int au_index, TMediaBuffer& buffer)
{
    wostringstream name;
    name << outputDir << L"/au_" << setw(4) << setfill(L'0') << au_index << L".h265";
    ofstream file(name.str(), ios::binary | ios::trunc);
    if (file)
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.dataSize());
}

// -----------------------------------------------------------------------

bool parse_h265_stream(const wstring& inputFile, const wstring& outputDir)
{
    try {
        TTranscoderW transcoder;
        transcoder
            .addInput(
                TMediaSocketW()
                    .file(inputFile)
            )
            .addOutput(
                // Pull-mode output: no file, just a video pin to receive AUs
                TMediaSocketW()
                    .addPin(TMediaPin().streamInfo(
                        TVideoStreamInfo().streamType(StreamType::H265)))
            )
            .open();

        if (!makeDir(outputDir))
        {
            wcerr << L"Cannot create output directory: " << outputDir << endl;
            return false;
        }

        int32_t outputIndex = 0;
        TMediaSample accessUnit;

        int au_index = 0;
        while (transcoder.pull(outputIndex, accessUnit))
        {
            auto buf = accessUnit.buffer();
            wcout << L"AU #" << au_index << L", " << buf.dataSize() << L" bytes" << endl;
            write_au_file(outputDir, au_index, buf);
            print_nalus(buf);
            ++au_index;
        }

        const auto error = transcoder.error();
        if (!(error.facility() == primo::error::ErrorFacility::Codec &&
              error.code()     == primo::codecs::CodecError::EOS))
            printError(L"Transcoder pull", error);

        transcoder.close();
        return true;

    } catch (const TAVBlocksException& ex) {
        wcerr << L"AVBlocks error: " << ex.what() << endl;
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
    return parse_h265_stream(opt.input_file, opt.output_dir) ? 0 : 1;
}
