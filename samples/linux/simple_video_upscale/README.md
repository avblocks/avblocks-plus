# simple_video_upscale

Scales a 480p (854x480) video up to Full HD 1080p (1920x1080) using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks video upscaling workflow:

1. Initialize the AVBlocks library (`TLibrary`)
2. Create a `TMediaInfo` object to probe the input file
3. Create an input `TMediaSocket` from the probed media info
4. Clone the input socket (`TMediaSocket::clone()`) to start with an identical output configuration
5. Set the output frame size to 1920x1080 on the output video pin
6. Set the resize interpolation method to `Cubic`, best for upscaling quality
7. Configure a `TTranscoder` with the input and output sockets
8. Run the transcode operation with `transcode()` (open, run and close in one call)

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```sh
cmake --preset debug-x64        # or debug-arm64
cmake --build --preset debug-x64 --target simple_video_upscale
```

The built executable will be placed in `bin/x64/simple_video_upscale` (or `bin/arm64/simple_video_upscale`).

## Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (480p, 854x480):

    ```sh
    cd samples/linux/simple_video_upscale
    curl -L -o AndrewConnolly_2014.mp4 \
        https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```sh
    ../../../bin/x64/simple_video_upscale    # or ../../../bin/arm64/simple_video_upscale
    ```

3. The upscaled output file `AndrewConnolly_2014_1080p.mp4` will be created in the `samples/linux/simple_video_upscale` directory.

## Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014_1080p.mp4` paths are hardcoded in `simple_video_upscale.cpp`. They are relative to the working directory from which the executable is run.
- An existing `AndrewConnolly_2014_1080p.mp4` is deleted before the transcoder runs, because the transcoder fails if the output file exists.
- The output video stream is resized to 1920x1080 (Full HD) using `InterpolationMethod::Cubic`, which gives the best upscaling quality at the cost of speed. The audio stream is copied from the source as is.
