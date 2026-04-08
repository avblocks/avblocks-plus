## demux_webm_file

Extract the first audio and video elementary stream from a WebM container and save each stream into a separate WebM file.

### Command Line

```sh
demux_webm_file --input <input_webm_file> --output <output_webm_file_name_without_extension>
```

###	Examples

List options:

```sh
./bin/x64/demux_webm_file --help
Usage: demux_webm_file -i <input_webm_file> -o <output_webm_file_name_without_extension>
  -h,    --help
  -i,    --input    Input webm file.
  -o,    --output   Output webm filename (without extension).
```

Extract the audio and video elementary streams from the input file `./assets/mov/big-buck-bunny_trailer_vp8_vorbis.webm` into output files named `big-buck-bunny_trailer_vp8_vorbis.vid.webm` and `big-buck-bunny_trailer_vp8_vorbis.aud.webm`:

```sh
mkdir -p ./output/demux_webm_file

./bin/x64/demux_webm_file \
    --input ./assets/mov/big-buck-bunny_trailer_vp8_vorbis.webm \
    --output ./output/demux_webm_file/big-buck-bunny_trailer_vp8_vorbis
```
