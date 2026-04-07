## dec_vp9_file

The dec_vp9_file sample shows how to decode VP9 video in IVF container to YUV uncompressed file.   

### Command Line

``` sh
./dec_vp9_file --input <file> --output <yuv_file>
```

###	Examples

List options:

```sh
./bin/x64/dec_vp9_file --help
Usage: dec_vp9_file -i <ivf file> -o <yuv file>
  -h,    --help
         --input    VP9/IVF input file.
         --output   YUV output file.
```

The following command decodes a VP9 video from `assets/vid/foreman_qcif_vp9.ivf` to `output/dec_vp9_file/foreman_qcif.yuv`:

```sh
mkdir -p ./output/dec_vp9_file

./bin/x64/dec_vp9_file \
    --input ./assets/vid/foreman_qcif_vp9.ivf \
    --output ./output/dec_vp9_file/foreman_qcif.yuv
```
