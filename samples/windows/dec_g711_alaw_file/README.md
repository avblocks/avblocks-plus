## dec_g711_alaw_file

Decode G.711 A-law WAV file to PCM WAV file.

### Command Line

```powershell
./dec_g711_alaw_file --input <g711 alaw wav file> --output <wav file>
```

### Examples

List options:

```powershell
./bin/x64/dec_g711_alaw_file --help

dec_g711_alaw_file --input <g711 alaw wav file> --output <wav file>
  -h,    --help
  -i,    --input    input G.711 A-law WAV file
  -o,    --output   output PCM WAV file
```

Decode the input file `./assets/aud/express-dictate_8000_s8_1ch_alaw.wav` into output file `./output/dec_g711_alaw_file/express-dictate_8000_s16_1ch_pcm.wav`:

```powershell
mkdir -Force -Path ./output/dec_g711_alaw_file

./bin/x64/dec_g711_alaw_file `
  --input ./assets/aud/express-dictate_8000_s8_1ch_alaw.wav `
  --output ./output/dec_g711_alaw_file/express-dictate_8000_s16_1ch_pcm.wav
```
