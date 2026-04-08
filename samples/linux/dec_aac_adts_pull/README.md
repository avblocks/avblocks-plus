## dec_aac_adts_pull

Decode AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::pull` and save output to WAV file.

### Command Line

```sh
./dec_aac_adts_pull --input <aac file> --output <wav file>
```

###	Examples

List options:

```sh
./bin/x64/dec_aac_adts_pull --help

dec_aac_adts_pull --input <aac file> --output <wav file>
  -h,    --help
  -i,    --input    input AAC file
  -o,    --output   output WAV file
```

The following example encode input file `..\assets\aud\Hydrate-Kenny_Beltrey.adts.aac` into output file `Hydrate-Kenny_Beltrey.wav`:

```sh
mkdir -p ./output/dec_aac_adts_pull

./bin/x64/dec_aac_adts_pull \
  --input ./assets/aud/Hydrate-Kenny_Beltrey.adts.aac \
  --output ./output/dec_aac_adts_pull/Hydrate-Kenny_Beltrey.wav
```

