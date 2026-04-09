## enc_g711_ulaw_file

Encode WAV file to G.711 U-law WAV file.

### Command Line

```powershell
enc_g711_ulaw_file --input <wav file> --output <g711 ulaw wav file>
```

### Examples

List options:

```powershell
./bin/x64/enc_g711_ulaw_file --help
Usage: enc_g711_ulaw_file --input <wav file> --output <g711 ulaw wav file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output G.711 U-law WAV file
```

Encode the input file `./assets/aud/express-dictate_8000_s16_1ch_pcm.wav` into output file `./output/enc_g711_ulaw_file/express-dictate_g711_ulaw.wav`:

```powershell
mkdir -Force -Path ./output/enc_g711_ulaw_file

./bin/x64/enc_g711_ulaw_file `
    --input ./assets/aud/express-dictate_8000_s16_1ch_pcm.wav `
    --output ./output/enc_g711_ulaw_file/express-dictate_g711_ulaw.wav
```
