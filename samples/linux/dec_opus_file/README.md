## dec_opus_file

Decode Opus OGG file and save output to WAV file.

### Command Line

```sh
./dec_opus_file --input <opus file> --output <wav file>
```

###	Examples

List options:

```sh
./bin/x64/dec_opus_file --help

dec_opus_file --input <opus file> --output <wav file>
  -h,    --help
  -i,    --input    input Opus file
  -o,    --output   output WAV file
```

The following example decodes input file `./assets/aud/Everybody-TBB.opus` into output file `Everybody-TBB.wav`:

```sh
mkdir -p ./output/dec_opus_file

./bin/x64/dec_opus_file \
  --input ./assets/aud/Everybody-TBB.opus \
  --output ./output/dec_opus_file/Everybody-TBB.wav
```
