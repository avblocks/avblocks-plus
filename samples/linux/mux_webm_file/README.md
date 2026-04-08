## mux_webm_file

Multiplex two single-stream WebM files containing Vorbis (audio) and VP8 (video) streams into a WebM (container) file.

### Command Line

```sh
mux_webm_file --audio <Vorbis_file>.webm --video <VP8_file>.webm --output <output>.webm
```

###	Examples

List options:

```sh
./bin/x64/mux_webm_file --help

Usage: mux_webm_file --audio <input_Vorbis> --video <input_VP8> --output <output.webm>
  -?,    --help
  -a,    --audio    input Vorbis files. Can be used multiple times
  -v,    --video    input VP8 files. Can be used multiple times
  -o,    --output   output file
```

Mux the WebM files `big-buck-bunny_trailer_vp8_vorbis.aud.webm` and `big-buck-bunny_trailer_vp8_vorbis.vid.webm` into the WebM file `big-buck-bunny_trailer.webm`: 

```sh
mkdir -p ./output/mux_webm_file

./bin/x64/mux_webm_file \
    --audio ./assets/aud/big-buck-bunny_trailer_vp8_vorbis.aud.webm \
    --video ./assets/vid/big-buck-bunny_trailer_vp8_vorbis.vid.webm \
    --output ./output/mux_webm_file/big-buck-bunny_trailer.webm 
```
