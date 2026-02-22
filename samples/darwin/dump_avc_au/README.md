## dump_avc_au

The `dump_avc_au` sample shows how to split an H.264 (AVC) elementary stream to access units (AU). The sample accepts an input H.264 file and an output folder. The sample reads the input file and writes the H.264 access units to the output folder, each AU is written in a separate file. It also displays the NAL units within each access unit.

### Command Line

```sh
./dump_avc_au --input <h264-file> --output <folder>
```

or short form:

```sh
./dump_avc_au -i <h264-file> -o <folder>
```

### Examples

List options:

```sh
./bin/x64/dump_avc_au --help
Usage: dump_avc_au --input <h264-file> --output <folder>
  -h,    --help
  -i,    --input    input file (AVC/H.264)
  -o,    --output   output directory
```

The following command extracts the H.264 access units from the `foreman_qcif.h264` video and writes them to the folder `output/dump_avc_au` as separate files (`au_####.h264`):

```sh
mkdir -p ./output/dump_avc_au

./bin/x64/dump_avc_au \
  --input ./assets/vid/foreman_qcif.h264 \
  --output ./output/dump_avc_au
```
