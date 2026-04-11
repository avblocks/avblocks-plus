#include <primo/avblocks/avb++.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::codecs;
using namespace primo::avblocks::modern;

const char* metaPictureTypeName(MetaPictureType::Enum pictype)
{
    switch (pictype)
    {
        case MetaPictureType::Other:               return "Other";
        case MetaPictureType::FileIcon:            return "FileIcon";
        case MetaPictureType::OtherFileIcon:       return "OtherFileIcon";
        case MetaPictureType::FrontCover:          return "FrontCover";
        case MetaPictureType::BackCover:           return "BackCover";
        case MetaPictureType::LeafletPage:         return "LeafletPage";
        case MetaPictureType::Media:               return "Media";
        case MetaPictureType::LeadArtist:          return "LeadArtist";
        case MetaPictureType::Artist:              return "Artist";
        case MetaPictureType::Conductor:           return "Conductor";
        case MetaPictureType::Band:                return "Band";
        case MetaPictureType::Composer:            return "Composer";
        case MetaPictureType::TextWriter:          return "TextWriter";
        case MetaPictureType::RecordingLocation:   return "RecordingLocation";
        case MetaPictureType::DuringRecording:     return "DuringRecording";
        case MetaPictureType::DuringPerformance:   return "DuringPerformance";
        case MetaPictureType::VideoCapture:        return "VideoCapture";
        case MetaPictureType::BrightColoredFish:   return "BrightColoredFish";
        case MetaPictureType::Illustration:        return "Illustration";
        case MetaPictureType::ArtistLogotype:      return "ArtistLogotype";
        case MetaPictureType::PublisherLogotype:   return "PublisherLogotype";
        default:                                   return "unknown";
    }
}

void printMetadata(TMetadata& meta)
{
    if (!meta.valid())
    {
        cout << "No metadata is found" << endl;
        return;
    }

    cout << "Metadata" << endl;
    cout << "--------" << endl;

    auto attlist = meta.attributes();
    cout << attlist.count() << " attributes:" << endl;

    for (int i = 0; i < attlist.count(); ++i)
    {
        auto attr = attlist.at(i);
        cout << setw(15) << left << attr.name() << ": " << attr.value() << endl;
    }
    cout << endl;

    auto piclist = meta.pictures();
    cout << piclist.count() << " pictures:" << endl;
    for (int i = 0; i < piclist.count(); ++i)
    {
        auto pic = piclist.at(i);
        cout << "#" << (i + 1)
             << " mime: "   << pic.mimeType()
             << "; size: "  << pic.dataSize()
             << "; type: "  << metaPictureTypeName(pic.pictureType()) << endl
             << "description: " << pic.description() << endl;
    }
    cout << endl;
}

void savePicture(TMetaPicture& pic, const string& baseFilename)
{
    string filename(baseFilename);

    const char* mime = pic.mimeType();
    if (0 == strcmp(mime, MimeType::Jpeg))
        filename += ".jpg";
    else if (0 == strcmp(mime, MimeType::Png))
        filename += ".png";
    else if (0 == strcmp(mime, MimeType::Gif))
        filename += ".gif";
    else if (0 == strcmp(mime, MimeType::Tiff))
        filename += ".tiff";
    else
        return;

    ofstream out(filename, ios::out | ios::binary | ios::trunc);
    if (out)
        out.write(reinterpret_cast<const char*>(pic.data()), pic.dataSize());
}

void savePictures(TMetadata& meta, const string& inputFile)
{
    auto piclist = meta.pictures();
    if (!meta.valid() || piclist.count() == 0)
        return;

    string baseFilename = inputFile + ".pic";

    for (int i = 0; i < piclist.count(); ++i)
    {
        char num[8];
        snprintf(num, sizeof(num), "%d", i + 1);
        string picname = baseFilename + num;
        auto pic = piclist.at(i);
        savePicture(pic, picname);
    }
}

bool metaInfo(Options& opt)
{
    TMediaInfo mi;
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
        printError("MediaInfo open", mi.error());
        return false;
    }
}

int main(int argc, char* argv[])
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
