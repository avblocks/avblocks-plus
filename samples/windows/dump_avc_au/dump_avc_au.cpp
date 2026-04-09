#include "stdafx.h"

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

static NALUType     nal_unit_type    (uint8_t b) { return static_cast<NALUType>   ((b & 0x1f)); }
static NALUPriority nal_unit_ref_idc (uint8_t b) { return static_cast<NALUPriority>((b & 0x60) >> 5); }

#define MAP_ENUM_VALUE(p) strings[p] = #p

static wostream& operator<<(wostream& out, NALUType value)
{
    static map<NALUType, wstring> strings;
    if (strings.empty())
    {
        strings[SLICE]    = L"SLICE";    strings[DPA]      = L"DPA";
        strings[DPB]      = L"DPB";     strings[DPC]      = L"DPC";
        strings[IDR]      = L"IDR";     strings[SEI]      = L"SEI";
        strings[SPS]      = L"SPS";     strings[PPS]      = L"PPS";
        strings[AUD]      = L"AUD";     strings[EOSEQ]    = L"EOSEQ";
        strings[EOSTREAM] = L"EOSTREAM"; strings[FILL]    = L"FILL";
    }
    return out << strings[value];
}

static wostream& operator<<(wostream& out, NALUPriority value)
{
    static map<NALUPriority, wstring> strings;
    if (strings.empty())
    {
        strings[DISPOSABLE] = L"DISPOSABLE"; strings[LOW]     = L"LOW";
        strings[HIGH]       = L"HIGH";       strings[HIGHEST] = L"HIGHEST";
    }
    return out << strings[value];
}

static void print_nalu_header(const uint8_t* data)
{
    wcout << left << L"  " << setw(8) << nal_unit_type(*data) << L": "
          << nal_unit_ref_idc(*data) << L"\n";
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
    name << outputDir << L"/au_" << setw(4) << setfill(L'0') << au_index << L".h264";
    ofstream file(name.str(), ios::binary | ios::trunc);
    if (file)
        file.write(reinterpret_cast<const char*>(buffer.data()), buffer.dataSize());
}

// -----------------------------------------------------------------------

bool parse_h264_stream(const wstring& inputFile, const wstring& outputDir)
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
                    .addPin(TMediaPin().streamInfo(TVideoStreamInfo()))
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
    return parse_h264_stream(opt.input_file, opt.output_dir) ? 0 : 1;
}
