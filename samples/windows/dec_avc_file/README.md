## dec_avc_file

The dec_avc_file sample shows how to decode AVC/H.264 Annex B compressed file to YUV uncompressed file.

### Command Line

```powershell
./dec_avc_file --input <file> --output <yuv_file>
```

### Examples

List options:

```powershell
./bin/x64/dec_avc_file --help
Usage: dec_avc_file -i <h264 file> -o <yuv file>
  -h,    --help
         --input    H264 input file.
         --output   YUV output file.
```

The following command decodes an AVC/H.264 video from `assets/vid/foreman_qcif.h264` to `output/dec_avc_file/foreman_qcif.yuv`:

```powershell
mkdir -Force -Path ./output/dec_avc_file

./bin/x64/dec_avc_file `
    --input ./assets/vid/foreman_qcif.h264 `
    --output ./output/dec_avc_file/foreman_qcif.yuv
```
