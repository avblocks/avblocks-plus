## enc_avc_file

Encode raw YUV video file to AVC / H.264 Annex B video file using `Transcoder::run`.

### Command Line

```powershell
./enc_avc_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264> [--colors] [--help]
```

### Examples

List options:

```powershell
./bin/x64/enc_avc_file --help
enc_avc_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264> [--colors]
  -h,    --help
  -i,    --input    input YUV file
  -o,    --output   output H264 file
  -r,    --rate     input frame rate
  -f,    --frame    input frame sizes <width>x<height>
  -c,    --color    input color format. Use --colors to list all supported color formats
         --colors   list COLOR constants
```

Encode a raw YUV video from `./assets/vid/foreman_qcif.yuv` to a H.264 video in `./output/enc_avc_file/foreman_qcif.h264`:
```powershell
mkdir -Force -Path ./output/enc_avc_file

./bin/x64/enc_avc_file `
  --input ./assets/vid/foreman_qcif.yuv `
  --output ./output/enc_avc_file/foreman_qcif.h264 `
  --frame 176x144 `
  --rate 30 `
  --color yuv420
```
