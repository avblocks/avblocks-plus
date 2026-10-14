# simple_video_crop

Crops a 16:9 video to 4:3 using AVBlocks Transcoder with hardcoded input and output file paths.

This sample demonstrates the basic AVBlocks video cropping workflow:

1. Initialize the AVBlocks library (`TLibrary`)
2. Create a `TMediaInfoW` object to probe the input file
3. Create an input `TMediaSocketW` from the probed media info
4. Clone the input socket (`TMediaSocketW::clone()`) to start with an identical output configuration
5. Set the output frame size to 640x480 and the display ratio to 4:3
6. Set `Crop.Left` / `Crop.Right` parameters on the output video pin to cut the 854x480 source down to 640x480
7. Configure a `TTranscoderW` with the input and output sockets
8. Run the transcode operation with `transcode()` (open, run and close in one call)

No command line parsing is used — the input and output file paths are hardcoded in the source code.

## Building

From the repository root:

```powershell
cmake --preset debug-x64        # or debug-arm64
cmake --build --preset debug-x64 --target simple_video_crop
```

The built executable will be placed in `bin\x64\simple_video_crop.exe` (or `bin\arm64\simple_video_crop.exe`).

## Running

1. Download the sample video, the MP4 file from the TED talk video [What's the next window into our universe?](https://archive.org/details/AndrewConnolly_2014) by Andrew Connolly (Wide 480p, 854x480, 16:9):

    ```powershell
    cd samples\windows\simple_video_crop
    Invoke-WebRequest -Uri "https://archive.org/download/AndrewConnolly_2014/AndrewConnolly_2014.mp4" -OutFile AndrewConnolly_2014.mp4
    ```

2. Run the sample from the sample directory (the input/output file paths are relative to the working directory):

    ```powershell
    ..\..\..\bin\x64\simple_video_crop.exe    # or ..\..\..\bin\arm64\simple_video_crop.exe
    ```

3. The cropped output file `AndrewConnolly_2014_cropped.mp4` will be created in the `samples\windows\simple_video_crop` directory.

## Notes

- The input file `AndrewConnolly_2014.mp4` and output file `AndrewConnolly_2014_cropped.mp4` paths are hardcoded in `simple_video_crop.cpp`. They are relative to the working directory from which the executable is run.
- An existing `AndrewConnolly_2014_cropped.mp4` is deleted before the transcoder runs, because the transcoder fails if the output file exists.
- The source video is 16:9 at 854x480. To crop it to 4:3 at 640x480, (854 - 640) / 2 = 107 pixels are cropped from the left and right edges. The audio stream is copied from the source as is.
