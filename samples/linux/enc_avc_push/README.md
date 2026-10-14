## enc_avc_push

Encode raw YUV video file to AVC / H.264 Annex B video file using `Transcoder::push`.

### Command Line

```sh
./enc_avc_push --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264> [--colors]	[--help]
 ```

### Examples (x64)

List options:

```sh
./bin/x64/enc_avc_push --help
enc_avc_push --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264> [--colors]
  -h,    --help
  -i,    --input    input YUV file
  -o,    --output   output H264 file
  -r,    --rate     input frame rate
  -f,    --frame    input frame sizes <width>x<height>
  -c,    --color    input color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR constants
```

Encode a raw YUV video from `./assets/vid/foreman_qcif.yuv` to a H.264 video in `./output/enc_avc_push/foreman_qcif.h264`:
  
```sh
mkdir -p ./output/enc_avc_push

./bin/x64/enc_avc_push \
  --input ./assets/vid/foreman_qcif.yuv \
  --output ./output/enc_avc_push/foreman_qcif.h264 \
  --frame 176x144 \
  --rate 30 \
  --color yuv420
```

### Examples (arm64)

List options:

```sh
./bin/arm64/enc_avc_push --help
enc_avc_push --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264> [--colors]
  -h,    --help
  -i,    --input    input YUV file
  -o,    --output   output H264 file
  -r,    --rate     input frame rate
  -f,    --frame    input frame sizes <width>x<height>
  -c,    --color    input color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR constants
```

Encode a raw YUV video from `./assets/vid/foreman_qcif.yuv` to a H.264 video in `./output/enc_avc_push/foreman_qcif.h264`:
  
```sh
mkdir -p ./output/enc_avc_push

./bin/arm64/enc_avc_push \
  --input ./assets/vid/foreman_qcif.yuv \
  --output ./output/enc_avc_push/foreman_qcif.h264 \
  --frame 176x144 \
  --rate 30 \
  --color yuv420
```
