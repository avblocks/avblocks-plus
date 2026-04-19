## video_pad

Add black border padding around a video.

### Command Line

```powershell
video_pad -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>] [-l <left>] [-r <right>] [-t <top>] [-b <bottom>] [-c <color>]
```

### Examples

List options:

```powershell
./bin/x64/video_pad --help
Usage: video_pad -i <mp4 file> -o <mp4 file> [-w <width>] [-h <height>] [-l <left>] [-r <right>] [-t <top>] [-b <bottom>] [-c <color>]
  -h,    --help
         --input    MP4 input file.
         --output   MP4 output file.
         --width    Target width (pixels).
         --height   Target height (pixels).
         --left     Left padding (pixels).
         --right    Right padding (pixels).
         --top      Top padding (pixels).
         --bottom   Bottom padding (pixels).
         --color    Padding color (ARGB32).
```

Add 100 pixels of black padding on all sides of `./assets/vid/big_buck_bunny_trailer.vid.mp4` and save to `./output/video_pad/big_buck_bunny_padded.mp4`:

```powershell
mkdir -Force -Path ./output/video_pad

./bin/x64/video_pad `
    --input ./assets/vid/big_buck_bunny_trailer.vid.mp4 `
    --output ./output/video_pad/big_buck_bunny_padded.mp4 `
    --left 100 `
    --right 100 `
    --top 100 `
    --bottom 100
```
