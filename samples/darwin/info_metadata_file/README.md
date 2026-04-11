## info_metadata_file

Extract metadata information from a media file.   


### Command Line

```bash
info_metadata_file --input <avfile>
```

###	Examples

List options:

```sh
./bin/x64/info_metadata_file --help

Usage: info_metadata_file --input <file>
  -?,    --help
  -i,    --input   file; if no input is specified a default input file is used.
```

Extract the metadata from the `Hydrate-Kenny_Beltrey.ogg` song:
    
```sh    
./bin/x64/info_metadata_file --input ./assets/aud/Hydrate-Kenny_Beltrey.ogg
```   
