## enc_hevc_pull

Encode raw YUV video file to HEVC / H.265 Annex B video file using `Transcoder::pull`.  

### Command Line

```sh
./enc_hevc_pull --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h265> [--colors]	[--help]
 ```

###	Examples

List options:

```sh
./bin/x64/enc_hevc_pull --help
enc_hevc_pull --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h265> [--colors]
  -h,    --help
  -i,    --input    input YUV file
  -o,    --output   output H265 file
  -r,    --rate     input frame rate
  -f,    --frame    input frame sizes <width>x<height>
  -c,    --color    input color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR constants
```

Encode a raw YUV video from `./assets/vid/foreman_qcif.yuv` to an H.265 video in `./output/enc_hevc_pull/foreman_qcif.h265`:
  
```sh
mkdir -p ./output/enc_hevc_pull

./bin/x64/enc_hevc_pull \
  --input ./assets/vid/foreman_qcif.yuv \
  --output ./output/enc_hevc_pull/foreman_qcif.h265 \
  --frame 176x144 \
  --rate 30 \
  --color yuv420
```

