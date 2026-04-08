## dec_hevc_au

The `dec_hevc_au` sample shows how to decode an H.265 stream. The sample uses a sequence of files to simulate a stream of H.265 Access Units and a Transcoder object to decode the H.265 Access Units to raw YUV video frames.

### Command Line

``` sh
./dec_hevc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]
```

###	Examples

List options:

```sh
./bin/x64/dec_hevc_au --help
Usage: dec_hevc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]
  -h,    --help
  -i,    --input    input directory (contains sequence of compressed file)
  -o,    --output   output YUV file
  -r,    --rate     frame rate
  -c,    --color    output color format. Use --colors to list all supported color
                    formats
         --colors   list COLOR formats
```

The following command reads H.265 Access Unit files under the `assets/vid/foreman_qcif.h265.au/` directory and decodes them to raw video frames. The decoded video is written to `output/dec_hevc_au/decoded_352x288.yuv`:

``` sh
mkdir -p ./output/dec_hevc_au

./bin/x64/dec_hevc_au \
  --input ./assets/vid/foreman_qcif.h265.au \
  --output ./output/dec_hevc_au/decoded_352x288.yuv \
  --color yuv420

```
