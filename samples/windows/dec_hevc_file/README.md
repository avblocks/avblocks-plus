## dec_hevc_file

The `dec_hevc_file` sample shows how to decode an HEVC/H.265 Annex B compressed file to a raw uncompressed YUV video file.

### Command Line

```powershell
dec_hevc_file --input <h265 file> --output <yuv file>
```

###	Examples

List options:

```powershell
.\bin\x64\dec_hevc_file --help
Usage: dec_hevc_file -i <h265 file> -o <yuv file>
  -h,    --help
         --input    H265 input file.
         --output   YUV output file.
```

The following command decodes an HEVC/H.265 video from `assets/vid/foreman_qcif.h265` to `output/dec_hevc_file/foreman_qcif.yuv`:

```powershell
mkdir -Force -Path .\output\dec_hevc_file

.\bin\x64\dec_hevc_file `
    --input .\assets\vid\foreman_qcif.h265 `
    --output .\output\dec_hevc_file\foreman_qcif.yuv
```
