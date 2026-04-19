## video_framerate

Change the frame rate of a video from 24 fps to 30 fps.

### Command Line

```sh
video_framerate --input <mp4 file> --output <mp4 file> [--frame-rate <fps>]
```

### Examples

List options:

```sh
./bin/x64/video_framerate --help
Usage: video_framerate -i <mp4 file> -o <mp4 file> [-f <fps>]
  -h,    --help
         --input        MP4 input file.
         --output       MP4 output file.
         --frame-rate   Target frame rate (fps).
```

Change the frame rate of the input file `./assets/vid/big_buck_bunny_trailer.vid.mp4` to 30 fps and save to output file `./output/video_framerate/big_buck_bunny_30fps.mp4`:

```sh
mkdir -p ./output/video_framerate

./bin/x64/video_framerate \
    --input ./assets/vid/big_buck_bunny_trailer.vid.mp4 \
    --output ./output/video_framerate/big_buck_bunny_30fps.mp4 \
    --frame-rate 30.0
```
