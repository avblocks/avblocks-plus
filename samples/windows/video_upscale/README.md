## video_upscale

Upscale a video to Full HD (1920x1080) using bicubic interpolation method.

### Command Line

```powershell
video_upscale --input <mp4 file> --output <mp4 file> [--width <width>] [--height <height>]
```

### Examples

List options:

```powershell
./bin/x64/video_upscale --help
Usage: video_upscale -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>]
  -h,    --help
         --input    MP4 input file.
         --output   MP4 output file.
         --width    Target width (pixels).
         --height   Target height (pixels).
```

Upscale the input file `./assets/vid/big_buck_bunny_trailer.vid.mp4` to 1920x1080 and save to output file `./output/video_upscale/big_buck_bunny_1080p.mp4`:

```powershell
mkdir -Force -Path ./output/video_upscale

./bin/x64/video_upscale `
    --input ./assets/vid/big_buck_bunny_trailer.vid.mp4 `
    --output ./output/video_upscale/big_buck_bunny_1080p.mp4 `
    --width 1920 `
    --height 1080
```
