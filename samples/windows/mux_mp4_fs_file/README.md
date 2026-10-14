## mux_mp4_fs_file

Multiplex AAC audio and H.264 video into an MP4 container, with optional fast start (progressive download). The sample can:

* Mux audio and video into an MP4 file without fast start
* Mux audio and video into an MP4 file with fast start
* Remux an existing MP4 file into a fast-start MP4 file

The first H.264 video stream and the first AAC audio stream found in the input files are used. All other streams are ignored.

### Fast Start

By default the `moov` atom (the index of the media data) is written at the end of the MP4 file, after the `mdat` atom. Playback cannot start until the whole file has been downloaded.

With fast start enabled, the muxer runs a second pass that moves the `moov` atom before the `mdat` atom, so the file can be played while it is still downloading. The sample enables it by setting the `Param::Muxer::MP4::FastStart` parameter on the output socket with `TMediaSocketW::addParam`:

```cpp
output.addParam(Param::Muxer::MP4::FastStart, 1);
```

Socket parameters such as this one configure the container / muxer. Parameters for a codec or filter (for example `Param::Video::Crop::Left`) are set on a pin with `TMediaPin::addParam`.

### Command Line

```powershell
mux_mp4_fs_file --input <file>.mp4 --output <output>.mp4 [--fast_start]

mux_mp4_fs_file --input <file>.mp4 --input <file>.mp4 --output <output>.mp4 [--fast_start]
```

### Examples (x64)

List options:

```powershell
./bin/x64/mux_mp4_fs_file --help

Usage: mux_mp4_fs_file --input <file1> --input <file2> --output <output.mp4> [--fast_start]
  -?,    --help
  -f,    --fast_start   move the moov atom to the beginning of the file (fast
                        start)
  -i,    --input        input file (AAC, H264, MP4). Can be used multiple times
  -o,    --output       output file
```

Remux the MP4 file `big_buck_bunny_trailer.mp4` into the fast-start MP4 file `big_buck_bunny_trailer_fast_start.mp4`:

```powershell
mkdir -Force -Path ./output/mux_mp4_fs_file

./bin/x64/mux_mp4_fs_file `
    --input ./assets/mov/big_buck_bunny_trailer.mp4 `
    --output ./output/mux_mp4_fs_file/big_buck_bunny_trailer_fast_start.mp4 `
    --fast_start
```

Mux the MP4 files `big_buck_bunny_trailer.aud.mp4` and `big_buck_bunny_trailer.vid.mp4` into a fast-start MP4 file:

```powershell
./bin/x64/mux_mp4_fs_file `
    --input ./assets/aud/big_buck_bunny_trailer.aud.mp4 `
    --input ./assets/vid/big_buck_bunny_trailer.vid.mp4 `
    --output ./output/mux_mp4_fs_file/big_buck_bunny_trailer_mux_fast_start.mp4 `
    --fast_start
```

Run without arguments to use the default input and output files (fast start is enabled):

```powershell
./bin/x64/mux_mp4_fs_file
```

### Examples (arm64)

List options:

```powershell
./bin/arm64/mux_mp4_fs_file --help

Usage: mux_mp4_fs_file --input <file1> --input <file2> --output <output.mp4> [--fast_start]
  -?,    --help
  -f,    --fast_start   move the moov atom to the beginning of the file (fast
                        start)
  -i,    --input        input file (AAC, H264, MP4). Can be used multiple times
  -o,    --output       output file
```

Remux the MP4 file `big_buck_bunny_trailer.mp4` into the fast-start MP4 file `big_buck_bunny_trailer_fast_start.mp4`:

```powershell
mkdir -Force -Path ./output/mux_mp4_fs_file

./bin/arm64/mux_mp4_fs_file `
    --input ./assets/mov/big_buck_bunny_trailer.mp4 `
    --output ./output/mux_mp4_fs_file/big_buck_bunny_trailer_fast_start.mp4 `
    --fast_start
```

Mux the MP4 files `big_buck_bunny_trailer.aud.mp4` and `big_buck_bunny_trailer.vid.mp4` into a fast-start MP4 file:

```powershell
./bin/arm64/mux_mp4_fs_file `
    --input ./assets/aud/big_buck_bunny_trailer.aud.mp4 `
    --input ./assets/vid/big_buck_bunny_trailer.vid.mp4 `
    --output ./output/mux_mp4_fs_file/big_buck_bunny_trailer_mux_fast_start.mp4 `
    --fast_start
```

Run without arguments to use the default input and output files (fast start is enabled):

```powershell
./bin/arm64/mux_mp4_fs_file
```
