## enc_opus_file

How to encode WAV file to Opus OGG file.

### Command Line

```sh
enc_opus_file --input <wav file> --output <opus file>
```

###	Examples

List options:

```sh
./bin/x64/enc_opus_file --help
Usage: enc_opus_file --input <wav file> --output <opus file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output Opus file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_opus_file/equinox-48KHz.opus`:

```sh
mkdir -p ./output/enc_opus_file

./bin/x64/enc_opus_file \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_opus_file/equinox-48KHz.opus
```
