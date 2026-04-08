## mux_mp4_file

Multiplex two single-stream MP4 files containing AAC (audio) and H.264 (video) streams into an MP4 (container) file.

### Command Line

```sh
mux_mp4_file --audio <AAC_file>.mp4 --video <H264_file>.mp4 --output <output>.mp4
```

###	Examples

List options:

```sh
./bin/x64/mux_mp4_file --help

Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>
  -?,    --help
  -a,    --audio    input AAC files. Can be used multiple times
  -v,    --video    input H264 files. Can be used multiple times
  -o,    --output   output file
```

Mux the MP4 files `big_buck_bunny_trailer.aud.mp4` and `big_buck_bunny_trailer.vid.mp4` into the MP4 file `big_buck_bunny_trailer.mp4`: 

```sh
mkdir -p ./output/mux_mp4_file

./bin/x64/mux_mp4_file \
    --audio ./assets/aud/big_buck_bunny_trailer.aud.mp4 \
    --video ./assets/vid/big_buck_bunny_trailer.vid.mp4 \
    --output ./output/mux_mp4_file/big_buck_bunny_trailer.mp4 
```
