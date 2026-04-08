## re-encode

Take an MP4 input and re-encodes the audio and video streams back into MP4 output. Shows how to force encoding of individual streams even when it is not needed. 

### Command Line

```sh
re-encode [--input <input_file.mp4>] [--output <output_file.mp4>] [--audio <yes|no>] [--video <yes|no>]
```

###	Examples

List options:

```sh
./bin/x64/re-encode --help

Usage: re-encode --input inputFile.mp4 --output outputFile.mp4 [--audio yes|no] [--video yes|no]

  -h,    --help
  -i,    --input    input mp4 file
  -o,    --output   output mp4 file
  -a,    --audio    re-encode audio, yes|no
  -v,    --video    re-encode video, yes|no
```

Re-encode the video stream of an MP4 / H.264 clip.

```sh
mkdir -p ./output/re-encode

./bin/x64/re-encode \
  --audio no \
  --video yes \
  --input ./assets/mov/big_buck_bunny_trailer.mp4 \
  --output ./output/re-encode/big_buck_bunny_trailer.mp4
```
