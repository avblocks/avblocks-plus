## dec_avc_au

The `dec_avc_au` sample shows how to decode a H.264 stream. The sample uses a sequence of files to simulate a stream of H.264 Access Units and a Transcoder object to decode the H.264 Access Units to raw YUV video frames.

### Command Line

``` sh
./dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]
```

### Examples

List options:

```sh
./bin/x64/dec_avc_au --help
Usage: dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]
  -h,    --help
  -i,    --input    input directory (contains sequence of compressed file)
  -o,    --output   output YUV file
  -r,    --rate     frame rate
  -c,    --color    output color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR formats
```

The following command reads H.264 Access Unit files under the `assets/vid/foreman_qcif.h264.au/` directory and decodes them to raw video frames. The decoded video is written to `output/dec_avc_au/decoded_176x144.yuv`:

``` sh
mkdir -p ./output/dec_avc_au

./bin/x64/dec_avc_au \
  --input ./assets/vid/foreman_qcif.h264.au \
  --output ./output/dec_avc_au/decoded_176x144.yuv \
  --color yuv420
```
