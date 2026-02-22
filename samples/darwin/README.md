# Samples - C++ / macOS

## Decoding

### AAC 

> Advanced Audio Coding

#### dec_aac_adts_file

Decode AAC file in Audio Data Transport Stream (ADTS) format and save the output to WAV file.

See [dec_aac_adts_file](./dec_aac_adts_file) for details.

#### dec_aac_adts_pull

Decode AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::pull` and save output to WAV file.

See [dec_aac_adts_pull](./dec_aac_adts_pull) for details.

### AVC

> Advanced Video Coding / H.264

#### dec_avc_file

Decode H.264/AVC file and save the output to a raw YUV file.

See [dec_avc_file](./dec_avc_file) for details.

#### dec_avc_au

Decode H.264/AVC access units pushed one by one and save the output to a raw YUV file.

See [dec_avc_au](./dec_avc_au) for details.

## Encoding

### AVC

> Advanced Video Coding / H.264

#### enc_avc_file

Encode a raw YUV file to H.264/AVC file.

See [enc_avc_file](./enc_avc_file) for details.

#### enc_avc_pull

Encode a raw YUV file to H.264/AVC using `Transcoder::pull`.

See [enc_avc_pull](./enc_avc_pull) for details.

## Utility

#### dump_avc_au

Dump a H.264/AVC file as individual access unit files.

See [dump_avc_au](./dump_avc_au) for details.
