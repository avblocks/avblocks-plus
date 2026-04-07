## enc_vp9_file

Encode raw YUV video file to VP9 video in IVF (Duck IVF) container using `Transcoder::run`.  

### Command Line

```sh
./enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf> [--colors] [--help]
 ```

###	Examples

List options:

```sh
./bin/x64/enc_vp9_file --help
enc_vp9_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.ivf> [--colors]
  -h,    --help
  -i,    --input    input YUV file
  -o,    --output   output IVF file
  -r,    --rate     input frame rate
  -f,    --frame    input frame sizes <width>x<height>
  -c,    --color    input color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR constants
```

Encode a raw YUV video from `./assets/vid/foreman_qcif.yuv` to a VP9 video in `./output/enc_vp9_file/foreman_qcif.ivf`:
  
```sh
mkdir -p ./output/enc_vp9_file

./bin/x64/enc_vp9_file \
  --input ./assets/vid/foreman_qcif.yuv \
  --output ./output/enc_vp9_file/foreman_qcif.ivf \
  --frame 176x144 \
  --rate 30 \
  --color yuv420
```
