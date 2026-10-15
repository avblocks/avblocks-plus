# simple_video_pad

Fits a 16:9 video into a 4:3 frame by resizing and padding the picture, using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks video padding workflow:

1. Initialize the AVBlocks library (`TLibrary`)
2. Create a `TMediaInfo` object to probe the input file
3. Create an input `TMediaSocket` from the probed media info
4. Clone the input socket (`TMediaSocket::clone()`) to start with an identical output configuration
5. Set the output frame size to 640x480 and the display ratio to 4:3
6. Set `Pad.Top` / `Pad.Bottom` parameters on the output video pin so the original 16:9 picture is centered and padded to fit the 4:3 frame
7. Set the resize interpolation method to `Super` for best quality when downscaling
8. Configure a `TTranscoder` with the input and output sockets
9. Run the transcode operation with `transcode()` (open, run and close in one call)

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
cmake --preset debug-x64        # or debug-arm64
cmake --build --preset debug-x64 --target simple_video_pad
```

The built executable will be placed in `bin/x64/simple_video_pad` (or `bin/arm64/simple_video_pad`).

## Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (Wide 480p, 854x480, 16:9):

    ```sh
    cd samples/linux/simple_video_pad
    curl -L -o AndrewConnolly_2014.mp4 "https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4"
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    ../../../bin/x64/simple_video_pad    # or ../../../bin/arm64/simple_video_pad
    ```

3. The padded output file `AndrewConnolly_2014_640x480_Padded.mp4` will be created in the `samples/linux/simple_video_pad` directory.

## Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014_640x480_Padded.mp4` paths are hardcoded in `simple_video_pad.cpp`. They are relative to the working directory from which the executable is run.
- An existing `AndrewConnolly_2014_640x480_Padded.mp4` is deleted before the transcoder runs, because the transcoder fails if the output file exists.
- The source video is 16:9 at 854x480. The output frame is 640x480 (4:3), so the picture is squeezed to 640x360 (`640 * 9 / 16`) and padded by 60 pixels on top and bottom to preserve the original 16:9 aspect ratio within the 4:3 frame.
