## audio_upsample

How to upsample audio from 44.1 KHz to 48 KHz.

### Command Line

```powershell
audio_upsample --input <mp3 file> --output <mp3 file>
```

### Examples

List options:

```powershell
./bin/x64/audio_upsample --help
Usage: audio_upsample --input <mp3 file> --output <mp3 file>
  -h,    --help
  -i,    --input    input MP3 file
  -o,    --output   output MP3 file
```

Upsample the input file `./assets/aud/Hydrate-Kenny_Beltrey.mp3` into output file `./output/audio_upsample/Hydrate-Kenny_Beltrey-48khz.mp3`:

```powershell
mkdir -Force -Path ./output/audio_upsample

./bin/x64/audio_upsample `
    --input ./assets/aud/Hydrate-Kenny_Beltrey.mp3 `
    --output ./output/audio_upsample/Hydrate-Kenny_Beltrey-48khz.mp3
```
