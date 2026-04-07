## enc_mp3_pull

How to encode WAV file to MP3 file using Transcoder::pull.

### Command Line

```sh
enc_mp3_pull --input <wav file> --output <mp3 file>
```

###	Examples

List options:

```sh
./bin/x64/enc_mp3_pull --help
Usage: enc_mp3_pull --input <wav file> --output <mp3 file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output MP3 file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_mp3_pull/equinox-48KHz.mp3`:

```sh
mkdir -p ./output/enc_mp3_pull

./bin/x64/enc_mp3_pull \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_mp3_pull/equinox-48KHz.mp3
```
