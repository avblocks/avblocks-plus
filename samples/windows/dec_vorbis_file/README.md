## dec_vorbis_file

Decode Vorbis OGG file and save output to WAV file.

### Command Line

```powershell
dec_vorbis_file --input <ogg file> --output <wav file>
```

###	Examples

List options:

```powershell
.\bin\x64\dec_vorbis_file --help

dec_vorbis_file --input <ogg file> --output <wav file>
  -h,    --help
  -i,    --input    input OGG file
  -o,    --output   output WAV file
```

The following example decodes input file `.\assets\aud\Hydrate-Kenny_Beltrey.ogg` into output file `Hydrate-Kenny_Beltrey.wav`:

```powershell
mkdir -Force -Path .\output\dec_vorbis_file

.\bin\x64\dec_vorbis_file `
  --input .\assets\aud\Hydrate-Kenny_Beltrey.ogg `
  --output .\output\dec_vorbis_file\Hydrate-Kenny_Beltrey.wav
```
