## dump_hevc_au

The `dump_hevc_au` sample shows how to split an H.265 (HEVC) elementary stream into access units (AU). The sample accepts an input H.265 file and an output folder. It reads the input file and writes each H.265 access unit to the output folder as a separate file. It also displays the NAL units within each access unit.

### Command Line

```sh
./dump_hevc_au --input <h265-file> --output <folder>
```

or short form:

```sh
./dump_hevc_au -i <h265-file> -o <folder>
```

### Examples

List options:

```sh
./bin/x64/dump_hevc_au --help
Usage: dump_hevc_au --input <h265-file> --output <folder>
  -h,    --help
  -i,    --input    input file (HEVC/H.265)
  -o,    --output   output directory
```

The following command extracts the H.265 access units from the `foreman_qcif.h265` video and writes them to the folder `output/dump_hevc_au` as separate files (`au_####.h265`):

```sh
mkdir -p ./output/dump_hevc_au

./bin/x64/dump_hevc_au \
  --input ./assets/vid/foreman_qcif.h265 \
  --output ./output/dump_hevc_au
```

### NAL Unit Types

The sample recognises the HEVC NAL unit types defined in ITU-T H.265 Table 7-1:

| Type          | Value | Description                              |
|---------------|-------|------------------------------------------|
| TRAIL_N       | 0     | Trailing picture, non-reference          |
| TRAIL_R       | 1     | Trailing picture, reference              |
| TSA_N         | 2     | Temporal sub-layer access, non-reference |
| TSA_R         | 3     | Temporal sub-layer access, reference     |
| STSA_N        | 4     | Step-wise temporal sub-layer access, non-reference |
| STSA_R        | 5     | Step-wise temporal sub-layer access, reference |
| RADL_N        | 6     | Random access decodable leading, non-reference |
| RADL_R        | 7     | Random access decodable leading, reference |
| RASL_N        | 8     | Random access skipped leading, non-reference |
| RASL_R        | 9     | Random access skipped leading, reference |
| BLA_W_LP      | 16    | Broken link access with leading pictures |
| BLA_W_RADL    | 17    | Broken link access with RADL pictures    |
| BLA_N_LP      | 18    | Broken link access without leading pictures |
| IDR_W_RADL    | 19    | IDR with RADL pictures                   |
| IDR_N_LP      | 20    | IDR without leading pictures             |
| CRA_NUT       | 21    | Clean random access                      |
| VPS_NUT       | 32    | Video parameter set                      |
| SPS_NUT       | 33    | Sequence parameter set                   |
| PPS_NUT       | 34    | Picture parameter set                    |
| AUD_NUT       | 35    | Access unit delimiter                    |
| EOS_NUT       | 36    | End of sequence                          |
| EOB_NUT       | 37    | End of bitstream                         |
| FD_NUT        | 38    | Filler data                              |
| PREFIX_SEI    | 39    | Supplemental enhancement info (prefix)   |
| SUFFIX_SEI    | 40    | Supplemental enhancement info (suffix)   |
