# simple_video_framerate

Changes a video's frame rate from 24 fps to 30 fps using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks frame rate conversion workflow:

1. Initialize the AVBlocks library (`TLibrary`)
2. Create a `TMediaInfoW` object to probe the input file
3. Create an input `TMediaSocketW` from the probed media info
4. Clone the input socket (`TMediaSocketW::clone()`) to start with an identical output configuration
5. Set the output frame rate to 30 fps on the output video pin
6. Configure a `TTranscoderW` with the input and output sockets
7. Run the transcode operation with `transcode()` (open, run and close in one call)

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```powershell
cmake --preset debug-x64        # or debug-arm64
cmake --build --preset debug-x64 --target simple_video_framerate
```

The built executable will be placed in `bin\x64\simple_video_framerate.exe` (or `bin\arm64\simple_video_framerate.exe`).

## Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (Wide 480p, 854x480, 24 fps):

    ```powershell
    cd samples\windows\simple_video_framerate
    Invoke-WebRequest -Uri "https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4" -OutFile AndrewConnolly_2014.mp4
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```powershell
    ..\..\..\bin\x64\simple_video_framerate.exe    # or ..\..\..\bin\arm64\simple_video_framerate.exe
    ```

3. The converted output file `AndrewConnolly_2014_30fps.mp4` will be created in the `samples\windows\simple_video_framerate` directory.

## Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014_30fps.mp4` paths are hardcoded in `simple_video_framerate.cpp`. They are relative to the working directory from which the executable is run.
- An existing `AndrewConnolly_2014_30fps.mp4` is deleted before the transcoder runs, because the transcoder fails if the output file exists.
- The source video is encoded at 24 fps; the output video stream frame rate is set to 30 fps. The audio stream is copied from the source as is.
