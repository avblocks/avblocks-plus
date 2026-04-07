## demux_mp4_file

Extract the first audio and video elementary stream from an MP4 container and save each stream into a separate MP4 file.

### Command Line

```sh
demux_mp4_file --input <input_mp4_file> --output <output_mp4_file_name_without_extension>
```

###	Examples

List options:

```sh
./bin/x64/demux_mp4_file --help
Usage: demux_mp4_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>
  -h,    --help
  -i,    --input    Input mp4 file.
  -o,    --output   Output mp4 filename (without extension).
```

Extract the audio and video elementary streams from the input file `./assets/mov/big_buck_bunny_trailer.mp4` into output files named `big_buck_bunny_trailer.vid.mp4` and `big_buck_bunny_trailer.aud.mp4`:

```sh
mkdir -p ./output/demux_mp4_file

./bin/x64/demux_mp4_file \
    --input ./assets/mov/big_buck_bunny_trailer.mp4 \
    --output ./output/demux_mp4_file/big_buck_bunny_trailer
```

