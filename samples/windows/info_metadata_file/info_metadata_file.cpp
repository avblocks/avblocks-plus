#include "stdafx.h"

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::codecs;
using namespace primo::avblocks::modern;

const wchar_t* metaPictureTypeName(MetaPictureType::Enum pictype)
{
    switch (pictype)
    {
        case MetaPictureType::Other:               return L"Other";
        case MetaPictureType::FileIcon:            return L"FileIcon";
        case MetaPictureType::OtherFileIcon:       return L"OtherFileIcon";
        case MetaPictureType::FrontCover:          return L"FrontCover";
        case MetaPictureType::BackCover:           return L"BackCover";
        case MetaPictureType::LeafletPage:         return L"LeafletPage";
        case MetaPictureType::Media:               return L"Media";
        case MetaPictureType::LeadArtist:          return L"LeadArtist";
        case MetaPictureType::Artist:              return L"Artist";
        case MetaPictureType::Conductor:           return L"Conductor";
        case MetaPictureType::Band:                return L"Band";
        case MetaPictureType::Composer:            return L"Composer";
        case MetaPictureType::TextWriter:          return L"TextWriter";
        case MetaPictureType::RecordingLocation:   return L"RecordingLocation";
        case MetaPictureType::DuringRecording:     return L"DuringRecording";
        case MetaPictureType::DuringPerformance:   return L"DuringPerformance";
        case MetaPictureType::VideoCapture:        return L"VideoCapture";
        case MetaPictureType::BrightColoredFish:   return L"BrightColoredFish";
        case MetaPictureType::Illustration:        return L"Illustration";
        case MetaPictureType::ArtistLogotype:      return L"ArtistLogotype";
        case MetaPictureType::PublisherLogotype:   return L"PublisherLogotype";
        default:                                   return L"unknown";
    }
}

void printMetadata(TMetadata& meta)
{
    if (!meta.valid())
    {
        wcout << L"No metadata is found" << endl;
        return;
    }

    wcout << L"Metadata" << endl;
    wcout << L"--------" << endl;

    auto attlist = meta.attributes();
    wcout << attlist.count() << L" attributes:" << endl;

    for (int i = 0; i < attlist.count(); ++i)
    {
        auto attr = attlist.at(i);
        wcout << setw(15) << left << toWide(attr.name()) << L": " << toWide(attr.value()) << endl;
    }
    wcout << endl;

    auto piclist = meta.pictures();
    wcout << piclist.count() << L" pictures:" << endl;
    for (int i = 0; i < piclist.count(); ++i)
    {
        auto pic = piclist.at(i);
        wcout << L"#" << (i + 1)
             << L" mime: "   << toWide(pic.mimeType())
             << L"; size: "  << pic.dataSize()
             << L"; type: "  << metaPictureTypeName(pic.pictureType()) << endl
             << L"description: " << toWide(pic.description()) << endl;
    }
    wcout << endl;
}

void savePicture(TMetaPicture& pic, const wstring& baseFilename)
{
    wstring filename(baseFilename);

    const char* mime = pic.mimeType();
    if (0 == strcmp(mime, MimeType::Jpeg))
        filename += L".jpg";
    else if (0 == strcmp(mime, MimeType::Png))
        filename += L".png";
    else if (0 == strcmp(mime, MimeType::Gif))
        filename += L".gif";
    else if (0 == strcmp(mime, MimeType::Tiff))
        filename += L".tiff";
    else
        return;

    ofstream out(filename, ios::out | ios::binary | ios::trunc);
    if (out)
        out.write(reinterpret_cast<const char*>(pic.data()), pic.dataSize());
}

void savePictures(TMetadata& meta, const wstring& inputFile)
{
    auto piclist = meta.pictures();
    if (!meta.valid() || piclist.count() == 0)
        return;

    wstring baseFilename = inputFile + L".pic";

    for (int i = 0; i < piclist.count(); ++i)
    {
        wstring picname = baseFilename + to_wstring(i + 1);
        auto pic = piclist.at(i);
        savePicture(pic, picname);
    }
}

bool metaInfo(Options& opt)
{
    TMediaInfoW mi;
    mi.inputs(0).file(opt.inputFile);

    if (mi.tryOpen())
    {
        auto meta = mi.outputs(0).metadata();
        printMetadata(meta);
        savePictures(meta, opt.inputFile);
        return true;
    }
    else
    {
        printError(L"MediaInfo open", mi.error());
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;
    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
        case Parsed:  break;
    }

    TLibrary library;
    return metaInfo(opt) ? 0 : 1;
}
