## enc_mp3_push

How to encode WAV file to MP3 file using Transcoder::push.

### Command Line

```sh
enc_mp3_push --input <wav file> --output <mp3 file>
```

###	Examples

List options:

```sh
./bin/x64/enc_mp3_push --help
Usage: enc_mp3_push --input <wav file> --output <mp3 file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output MP3 file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_mp3_push/equinox-48KHz.mp3`:

```sh
mkdir -p ./output/enc_mp3_push

./bin/x64/enc_mp3_push \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_mp3_push/equinox-48KHz.mp3
```
