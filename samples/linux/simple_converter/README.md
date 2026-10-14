# simple_converter

Transcodes a media file from `H.264/AVC` to `H.265/HEVC` using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks transcoding workflow:

1. Initialize the AVBlocks library (`TLibrary`)
2. Create a `TMediaInfo` object to probe the input file
3. Create an input `TMediaSocket` from the probed media info
4. Clone the input socket (`MP4 / H.264 (AVC) + AAC`) to make the output socket and change its video stream type to `H.265 (HEVC)`
5. Configure a `TTranscoder` with the input and output sockets
6. Run the transcode operation with `transcode()` (open, run and close in one call)

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
cmake --preset debug-x64        # or debug-arm64
cmake --build --preset debug-x64 --target simple_converter
```

The built executable will be placed in `bin/x64/simple_converter` (or `bin/arm64/simple_converter`).

## Running

1. Download the sample video:

    ```sh
    cd samples/linux/simple_converter
    curl -L -o Wildlife_h264_aac.mp4 https://archive.org/download/WildlifeSampleVideo/Wildlife.mp4
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    ../../../bin/x64/simple_converter    # or ../../../bin/arm64/simple_converter
    ```

3. The transcoded output file `Wildlife_h265_aac.mp4` will be created in the `samples/linux/simple_converter` directory.

## Notes

- The input file `Wildlife_h264_aac.mp4` and output file `Wildlife_h265_aac.mp4` paths are hardcoded in `simple_converter.cpp`. They are relative to the working directory from which the executable is run.
- An existing `Wildlife_h265_aac.mp4` is deleted before the transcoder runs, because the transcoder fails if the output file exists.
- The output socket starts as a copy of the input (MP4 container with H.264 video and AAC audio) and the video stream type is changed to `StreamType::H265`, producing an MP4 container with H.265 video and AAC audio streams.
