## dec_mp3_file

Decode MP3 file and save output to WAV file.

### Command Line

```sh
./dec_mp3_file --input <mp3 file> --output <wav file>
```

###	Examples

List options:

```sh
./bin/x64/dec_mp3_file --help

dec_mp3_file --input <mp3 file> --output <wav file>
  -h,    --help
  -i,    --input    input MP3 file
  -o,    --output   output WAV file
```

The following example encode input file `..\assets\aud\Hydrate-Kenny_Beltrey.mp3` into output file `Hydrate-Kenny_Beltrey.wav`:

```sh
mkdir -p ./output/dec_mp3_file

./bin/x64/dec_mp3_file \
  --input ./assets/aud/Hydrate-Kenny_Beltrey.mp3 \
  --output ./output/dec_mp3_file/Hydrate-Kenny_Beltrey.wav
```

