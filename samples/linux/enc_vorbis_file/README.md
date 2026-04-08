## enc_vorbis_file

How to encode WAV file to Vorbis OGG file.

### Command Line

```sh
enc_vorbis_file --input <wav file> --output <ogg file>
```

###	Examples

List options:

```sh
./bin/x64/enc_vorbis_file --help
Usage: enc_vorbis_file --input <wav file> --output <ogg file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output OGG file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_vorbis_file/equinox-48KHz.ogg`:

```sh
mkdir -p ./output/enc_vorbis_file

./bin/x64/enc_vorbis_file \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_vorbis_file/equinox-48KHz.ogg
```
