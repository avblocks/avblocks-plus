## enc_mp3_file

How to encode WAV file to MP3 file.

### Command Line

```sh
enc_mp3_file --input <wav file> --output <mp3 file>
```

###	Examples

List options:

```sh
./bin/x64/enc_mp3_file --help
Usage: enc_mp3_file --input <wav file> --output <mp3 file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output MP3 file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_mp3_file/equinox-48KHz.mp3`:

```sh
mkdir -p ./output/enc_mp3_file

./bin/x64/enc_mp3_file \
    --input ./assets/aud/equinox-48KHz.wav \
    --output ./output/enc_mp3_file/equinox-48KHz.mp3
```
