## enc_opus_file

How to encode WAV file to Opus OGG file.

### Command Line

```powershell
enc_opus_file --input <wav file> --output <opus file>
```

### Examples (x64)

List options:

```powershell
./bin/x64/enc_opus_file --help
Usage: enc_opus_file --input <wav file> --output <opus file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output Opus file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_opus_file/equinox-48KHz.opus`:

```powershell
mkdir -Force -Path ./output/enc_opus_file

./bin/x64/enc_opus_file `
    --input ./assets/aud/equinox-48KHz.wav `
    --output ./output/enc_opus_file/equinox-48KHz.opus
```

### Examples (arm64)

List options:

```powershell
./bin/arm64/enc_opus_file --help
Usage: enc_opus_file --input <wav file> --output <opus file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output Opus file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_opus_file/equinox-48KHz.opus`:

```powershell
mkdir -Force -Path ./output/enc_opus_file

./bin/arm64/enc_opus_file `
    --input ./assets/aud/equinox-48KHz.wav `
    --output ./output/enc_opus_file/equinox-48KHz.opus
```
