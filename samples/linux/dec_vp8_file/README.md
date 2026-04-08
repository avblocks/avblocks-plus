## dec_vp8_file

The dec_vp8_file sample shows how to decode VP8 video in IVF container to YUV uncompressed file.   

### Command Line

``` sh
./dec_vp8_file --input <file> --output <yuv_file>
```

###	Examples

List options:

```sh
./bin/x64/dec_vp8_file --help
Usage: dec_vp8_file -i <ivf file> -o <yuv file>
  -h,    --help
         --input    VP8/IVF input file.
         --output   YUV output file.
```

The following command decodes a VP8 video from `assets/vid/foreman_qcif_vp8.ivf` to `output/dec_vp8_file/foreman_qcif.yuv`:

```sh
mkdir -p ./output/dec_vp8_file

./bin/x64/dec_vp8_file \
    --input ./assets/vid/foreman_qcif_vp8.ivf \
    --output ./output/dec_vp8_file/foreman_qcif.yuv
```
