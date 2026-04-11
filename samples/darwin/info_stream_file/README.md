## info_stream_file

Extract the audio / video stream information from a media file.   

### Command Line

```bash
info_stream_file --input <avfile>
```

###	Examples

List options:

```sh
./bin/x64/info_stream_file --help

Usage: info_stream_file --input <avfile>
  -h,    --help
  -i,    --input   file; if no input is specified a default input file is used.
```

List the audio and video streams of the `big_buck_bunny_trailer.mp4` movie trailer:
    
```sh    
./bin/x64/info_stream_file --input ./assets/mov/big_buck_bunny_trailer.mp4
```   
