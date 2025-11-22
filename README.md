# avblocks-plus (beta)

AVBlocks for Modern C++

# Overview

AVBlocks Plus provides a modern C++ wrapper around the AVBlocks SDK, offering a fluent API for audio and video transcoding operations. It simplifies common tasks like format conversion, encoding, decoding, and stream manipulation.

## Key Features

- **Fluent API**: Chain methods together for concise, readable code
- **Type Safety**: Modern C++ templates and smart pointers
- **Exception Handling**: Clear error reporting with AVBlocksException
- **Cross-Platform**: Support for Windows, macOS, and Linux
- **Hardware Acceleration**: Support for Intel QuickSync, NVIDIA NVENC, and AMD VCE

# Usage

## Basic Transcoding Example

```cpp
#include <primo/avblocks/avb++.h>

using namespace primo::codecs;
using namespace primo::avblocks::modern;

int main() {
    try {
        // Initialize library
        TLibrary library;
        
        // Simple file conversion
        TTranscoder()
            .allowDemoMode(true)
            .addInput(TMediaSocket().file("input.mp4"))
            .addOutput(TMediaSocket().file("output.wav")
                .streamType(StreamType::WAVE)
                .addPin(TMediaPin().audioStreamType(StreamType::LPCM)))
            .open()
            .run()
            .close();
            
        return 0;
    } catch (const TAVBlocksException& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
```

## Audio Decoding

Decode AAC to PCM audio:

```cpp
// Create input socket from AAC file
TMediaSocket inputSocket = TMediaSocket()
    .file("audio.aac");

// Create output socket for WAV file with LPCM audio
TMediaSocket outputSocket = TMediaSocket()
    .file("output.wav")
    .streamType(StreamType::WAVE)
    .addPin(TMediaPin().audioStreamType(StreamType::LPCM));

// Transcode
TTranscoder()
    .allowDemoMode(true)
    .addInput(inputSocket)
    .addOutput(outputSocket)
    .open()
    .run()
    .close();
```

## Video Encoding

Encode raw video to H.264:

```cpp
// Input: raw YUV video
TMediaSocket inputSocket = TMediaSocket()
    .file("raw_video.yuv")
    .addPin(TMediaPin()
        .videoStreamType(StreamType::UncompressedVideo)
        .videoWidth(1920)
        .videoHeight(1080)
        .videoFrameRate(30.0)
        .videoColorFormat(ColorFormat::YUV420));

// Output: H.264 in MP4 container
TMediaSocket outputSocket = TMediaSocket()
    .file("output.mp4")
    .streamType(StreamType::MP4)
    .addPin(TMediaPin()
        .videoStreamType(StreamType::H264)
        .videoWidth(1920)
        .videoHeight(1080)
        .videoFrameRate(30.0)
        .videoBitrate(5000000));

TTranscoder()
    .allowDemoMode(true)
    .addInput(inputSocket)
    .addOutput(outputSocket)
    .open()
    .run()
    .close();
```

## Using Presets

AVBlocks provides presets for common output formats:

```cpp
// Create output from preset
TMediaSocket outputSocket = TMediaSocket()
    .file("output.mp4")
    .preset("ipad.mp4.h264.720p");

TTranscoder()
    .allowDemoMode(true)
    .addInput(TMediaSocket().file("input.avi"))
    .addOutput(outputSocket)
    .open()
    .run()
    .close();
```

Available presets include:
- `ipad.mp4.h264.720p` - iPad-compatible 720p H.264
- `iphone.mp4.h264.480p` - iPhone-compatible 480p H.264
- `dvd.ntsc.4x3.mp2` - DVD-Video NTSC format
- `web.mp4.h264.720p` - Web-optimized 720p H.264
- And many more (see Preset documentation)

## Media Information

Get information about a media file:

```cpp
TMediaInfo info = TMediaInfo()
    .addInput(TMediaSocket().file("video.mp4"))
    .open();

// Access output sockets with stream information
for (int i = 0; i < info.outputs().count(); ++i) {
    MediaSocket* socket = info.outputs().at(i);
    
    // Iterate through pins (streams)
    for (int j = 0; j < socket->pins().count(); ++j) {
        MediaPin* pin = socket->pins().at(j);
        StreamInfo* streamInfo = pin->streamInfo();
        
        if (streamInfo->mediaType() == MediaType::Video) {
            auto videoInfo = static_cast<VideoStreamInfo*>(streamInfo);
            std::cout << "Video: " << videoInfo->frameWidth() 
                      << "x" << videoInfo->frameHeight() << std::endl;
        }
    }
}
```

## Hardware Acceleration

Enable hardware encoding:

```cpp
TMediaSocket outputSocket = TMediaSocket()
    .file("output.mp4")
    .streamType(StreamType::MP4)
    .addPin(TMediaPin()
        .videoStreamType(StreamType::H264)
        .addParam(IntParam(Param::HardwareEncoder, 
                          HardwareEncoder::Auto)));

// Or configure globally
TLibrary::config()->hardware()->setIntelMedia(true);
TLibrary::config()->hardware()->setNvenc(true);
```

## Error Handling

The modern API uses exceptions for error handling:

```cpp
try {
    TTranscoder transcoder = TTranscoder()
        .addInput(TMediaSocket().file("input.mp4"))
        .addOutput(TMediaSocket().file("output.wav"))
        .open();
        
    transcoder.run();
    transcoder.close();
    
} catch (const TAVBlocksException& ex) {
    std::cerr << "AVBlocks error: " << ex.what() << std::endl;
    // Access detailed error information
    const auto* errorInfo = ex.errorInfo();
    if (errorInfo) {
        std::cerr << "Facility: " << errorInfo->facility() << std::endl;
        std::cerr << "Code: " << errorInfo->code() << std::endl;
    }
} catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
}
```

## Stream Processing (Push/Pull)

For advanced scenarios, you can push/pull media samples:

```cpp
// Create decoder transcoder - input from file, output PCM without file
TTranscoder decoder = TTranscoder()
    .allowDemoMode(true)
    .addInput(TMediaSocket().file("input.aac"))
    .addOutput(TMediaSocket()
        .streamType(StreamType::LPCM)
        .addPin(TMediaPin()
            .audioStreamType(StreamType::LPCM)
            .channels(2)
            .sampleRate(48000)
            .bitsPerSample(16)))
    .open();

// Create WAV writer transcoder - input PCM without file, output to file
TTranscoder wavWriter = TTranscoder()
    .allowDemoMode(true)
    .addInput(TMediaSocket()
        .streamType(StreamType::LPCM)
        .addPin(TMediaPin()
            .audioStreamType(StreamType::LPCM)
            .channels(2)
            .sampleRate(48000)
            .bitsPerSample(16)))
    .addOutput(TMediaSocket()
        .file("output.wav")
        .streamType(StreamType::WAVE)
        .addPin(TMediaPin()
            .audioStreamType(StreamType::LPCM)
            .channels(2)
            .sampleRate(48000)
            .bitsPerSample(16)))
    .open();

// Pull-push decoding loop
int32_t outputIndex = 0;
TMediaSample pcmSample;

bool decoderEos = false;
while (!decoderEos) {
    // Pull PCM sample from decoder
    if (decoder.pull(outputIndex, pcmSample)) {
        // Push PCM sample to WAV writer
        wavWriter.push(0, pcmSample);
        continue;
    }

    // Check for end of stream
    const auto* error = decoder.error();
    if (error->facility() == primo::error::ErrorFacility::Codec &&
        error->code() == CodecError::EOS) {
        // Push null sample to signal EOS to WAV writer
        TMediaSample nullSample;
        wavWriter.push(0, nullSample);
        decoderEos = true;
    }
}

decoder.close();
wavWriter.close();
```

## API Reference

### Core Classes

- **TLibrary**: Initialize/shutdown AVBlocks, manage licensing
- **TTranscoder/TTranscoderW**: Main transcoding engine (ANSI/Wide character variants)
- **TMediaSocket/TMediaSocketW**: Input/output endpoint (file, stream, or elementary)
- **TMediaPin**: Elementary stream within a socket
- **TMediaInfo**: Analyze media files
- **TMediaSample**: Container for media data
- **TMediaBuffer**: Raw media data buffer

### Stream Configuration

- **StreamType**: Container formats (MP4, AVI, WAV, etc.)
- **ColorFormat**: Video color formats (YUV420, RGB, etc.)
- **ScanType**: Progressive or interlaced video
- **AudioChannelFlags**: Audio channel layouts

### Parameters

Configure encoding/decoding behavior:

```cpp
TMediaPin pin = TMediaPin()
    .addParam(IntParam(Param::Video::Bitrate, 5000000))
    .addParam(IntParam(Param::Encoder::Video::H264::Profile, 
                      H264Profile::High))
    .addParam(IntParam(Param::Encoder::Video::H264::Level, 41));
```

## Platform-Specific Notes

### Windows
- Use `TMediaSocketW` and `TTranscoderW` classes for wide-character (Unicode) file paths
- Link against `AVBlocks64.lib` 

### macOS
- Use `TMediaSocket` and `TTranscoder` classes (ANSI/UTF-8 paths)
- Link against `libAVBlocks.dylib`

### Linux  
- Use `TMediaSocket` and `TTranscoder` classes (ANSI/UTF-8 paths)
- Link against `libAVBlocks64.so`

# Development

## macOS

### Download AVBlocks Core and Assets

See [Download Core and Assets on macOS](./docs/download-avblocks-core-and-assets-mac.md) 

### Setup

See [Setup for macOS](./docs/setup-mac.md)

### Build

See [Build on macOS](./docs/build-mac.md)

### Run

See [README](./samples/darwin/README.md) in the `samples` subdirectory. 

## Linux

### Download AVBlocks Core and Assets

See [Download Core and Assets on Linux](./docs/download-avblocks-core-and-assets-linux.md) 

### Setup

See [Setup for Linux](./docs/setup-linux.md)

### Build

See [Build on Linux](./docs/build-linux.md)

### Run

See [README](./samples/linux/README.md) in the `samples` subdirectory. 

## Windows

### Download AVBlocks Core and Assets

See [Download Core and Assets on Windows](./docs/download-avblocks-core-and-assets-windows.md) 

### Setup

See [Setup for Windows](./docs/setup-windows.md)

### Build

See [Build on Windows](./docs/build-windows.md)

### Run

See [README](./samples/windows/README.md) in the `samples` subdirectory. 

# How to obtain Commercial License

See [License Options](https://avblocks.com/license/) for details.

We offer discounts for:

- Competitive product
- Startup
- Educational institution
- Open source project
