## enc_aac_adts_file

How to encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

### Command Line

```powershell
enc_aac_adts_file --input <wav file> --output <aac file>
```

###	Examples

List options:

```powershell
./bin/x64/enc_aac_adts_file --help

enc_aac_adts_file --input <wav file> --output <aac file>
  -h,    --help
  -i,    --input    input WAV file
  -o,    --output   output AAC file
```

Encode the input file `./assets/aud/equinox-48KHz.wav` into output file `./output/enc_aac_adts_file/equinox-48KHz.adts.aac`:

```powershell
mkdir -Force -Path ./output/enc_aac_adts_file

./bin/x64/enc_aac_adts_file `
    --input ./assets/aud/equinox-48KHz.wav `
    --output ./output/enc_aac_adts_file/equinox-48KHz.adts.aac
```
