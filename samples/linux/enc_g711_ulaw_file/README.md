## enc_g711_ulaw_file

Encode WAV file to G.711 μ-law WAV file.

### Command Line

```sh
enc_g711_ulaw_file --input <wav file> --output <g711 ulaw wav file>
```

###	Examples

List options:

```sh
./bin/x64/enc_g711_ulaw_file --help
Usage: enc_g711_ulaw_file --input <wav file> --output <g711 ulaw wav file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output G.711 μ-law WAV file
```

Encode the input file `./assets/aud/express-dictate_8000_s16_1ch_pcm.wav` into output file `./output/enc_g711_ulaw_file/express-dictate_g711_ulaw.wav`:

```sh
mkdir -p ./output/enc_g711_ulaw_file

./bin/x64/enc_g711_ulaw_file \
    --input ./assets/aud/express-dictate_8000_s16_1ch_pcm.wav \
    --output ./output/enc_g711_ulaw_file/express-dictate_g711_ulaw.wav
```
