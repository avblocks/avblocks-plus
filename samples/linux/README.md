# Samples - C++ / Linux

## Media Info

#### info_metadata_file

Extract metadata information (text attributes and embedded pictures) from a media file.

See [info_metadata_file](./info_metadata_file) for details.

#### info_stream_file

Extract the audio / video stream information from a media file.

See [info_stream_file](./info_stream_file) for details.

## Decoding

### AAC 

> Advanced Audio Coding

#### dec_aac_adts_file

Decode AAC file in Audio Data Transport Stream (ADTS) format and save the output to WAV file.

See [dec_aac_adts_file](./dec_aac_adts_file) for details.

#### dec_aac_adts_pull

Decode AAC file in Audio Data Transport Stream (ADTS) format using `Transcoder::pull` and save output to WAV file.

See [dec_aac_adts_pull](./dec_aac_adts_pull) for details.

### G.711

> ITU-T G.711 audio codec

#### dec_g711_alaw_file

Decode G.711 A-law WAV file to PCM WAV file.

See [dec_g711_alaw_file](./dec_g711_alaw_file) for details.

#### dec_g711_ulaw_file

Decode G.711 μ-law WAV file to PCM WAV file.

See [dec_g711_ulaw_file](./dec_g711_ulaw_file) for details.

#### enc_g711_alaw_file

Encode WAV file to G.711 A-law WAV file.

See [enc_g711_alaw_file](./enc_g711_alaw_file) for details.

#### enc_g711_ulaw_file

Encode WAV file to G.711 μ-law WAV file.

See [enc_g711_ulaw_file](./enc_g711_ulaw_file) for details.

### MP3

> MPEG-1 Layer 3 Audio

#### dec_mp3_file

Decode MP3 file and save output to WAV file.

See [dec_mp3_file](./dec_mp3_file) for details.

### Vorbis

> Vorbis Audio

#### dec_vorbis_file

Decode Vorbis OGG file and save output to WAV file.

See [dec_vorbis_file](./dec_vorbis_file) for details.

#### enc_vorbis_file

Encode a WAV file to Vorbis OGG file.

See [enc_vorbis_file](./enc_vorbis_file) for details.

### Opus

> Opus Audio

#### dec_opus_file

Decode Opus file and save output to WAV file.

See [dec_opus_file](./dec_opus_file) for details.

#### enc_opus_file

Encode a WAV file to Opus OGG file.

See [enc_opus_file](./enc_opus_file) for details.

### AVC

> Advanced Video Coding / H.264

#### dec_avc_file

Decode H.264/AVC file and save the output to a raw YUV file.

See [dec_avc_file](./dec_avc_file) for details.

#### dec_avc_au

Decode H.264/AVC access units pushed one by one and save the output to a raw YUV file.

See [dec_avc_au](./dec_avc_au) for details.

### HEVC

> High Efficiency Video Coding / H.265

#### dec_hevc_file

Decode a compressed HEVC / H.265 Annex B file to raw uncompressed YUV video file.

See [dec_hevc_file](./dec_hevc_file) for details.

#### dec_hevc_au

Decode HEVC / H.265 stream. The sample uses a sequence of files to simulate a stream of H.265 Access Units (AUs) and a Transcoder object to decode the AUs to raw YUV video frames.

See [dec_hevc_au](./dec_hevc_au) for details.

#### enc_hevc_file

Encode a raw YUV video file to HEVC / H.265 Annex B file.

See [enc_hevc_file](./enc_hevc_file) for details.

#### enc_hevc_pull

Encode a raw YUV video file to HEVC / H.265 Annex B file using `Transcoder::pull`.

See [enc_hevc_pull](./enc_hevc_pull) for details.

### VP8

#### dec_vp8_file

Decode VP8 video in IVF container and save output to YUV file.

See [dec_vp8_file](./dec_vp8_file) for details.

#### enc_vp8_file

Encode a raw YUV file to VP8 video in IVF container.

See [enc_vp8_file](./enc_vp8_file) for details.

### VP9

#### dec_vp9_file

Decode VP9 video in IVF container and save output to YUV file.

See [dec_vp9_file](./dec_vp9_file) for details.

#### enc_vp9_file

Encode a raw YUV file to VP9 video in IVF container.

See [enc_vp9_file](./enc_vp9_file) for details.

## Demuxing

### MP4

> MPEG-4 Part 14 / MPEG-4 Part 1

#### demux_mp4_file

Extract the first audio and video elementary stream from an MP4 container and save each stream into a separate MP4 file.

See [demux_mp4_file](./demux_mp4_file) for details.

### WebM

> Matroska / WebM

#### demux_webm_file

Extract the first audio and video elementary stream from a WebM container and save each stream into a separate WebM file.

See [demux_webm_file](./demux_webm_file) for details.

## Encoding

### AAC

> Advanced Audio Coding

#### enc_aac_adts_file

Encode a WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

See [enc_aac_adts_file](./enc_aac_adts_file) for details.

#### enc_aac_adts_pull

Encode a WAV file to AAC in Audio Data Transport Stream (ADTS) format using `Transcoder::pull`.

See [enc_aac_adts_pull](./enc_aac_adts_pull) for details.

#### enc_aac_adts_push

Encode a WAV file to AAC in Audio Data Transport Stream (ADTS) format using `Transcoder::push`.

See [enc_aac_adts_push](./enc_aac_adts_push) for details.

### AVC

> Advanced Video Coding / H.264

#### enc_avc_file

Encode a raw YUV file to H.264/AVC file.

See [enc_avc_file](./enc_avc_file) for details.

#### enc_avc_pull

Encode a raw YUV file to H.264/AVC using `Transcoder::pull`.

See [enc_avc_pull](./enc_avc_pull) for details.

### MP3

> MPEG-1 Layer 3 Audio

#### enc_mp3_file

Encode a WAV file to MP3 file.

See [enc_mp3_file](./enc_mp3_file) for details.

#### enc_mp3_pull

Encode a WAV file to MP3 file using `Transcoder::pull`.

See [enc_mp3_pull](./enc_mp3_pull) for details.

#### enc_mp3_push

Encode a WAV file to MP3 file using `Transcoder::push`.

See [enc_mp3_push](./enc_mp3_push) for details.

### Preset

#### enc_preset_file

Encode a raw YUV video file to a variety of output formats using an AVBlocks preset.

See [enc_preset_file](./enc_preset_file) for details.

## Muxing

#### mux_mp4_file

Multiplex two single-stream MP4 files containing AAC (audio) and H.264 (video) streams into an MP4 (container) file.

See [mux_mp4_file](./mux_mp4_file) for details.

#### mux_webm_file

Multiplex two single-stream WebM files containing Vorbis (audio) and VP8 (video) streams into a WebM (container) file.

See [mux_webm_file](./mux_webm_file) for details.

## Re-encoding

#### re-encode

Re-encode audio and video streams of an MP4 file.

See [re-encode](./re-encode) for details.

## Slideshow

#### slideshow

Create an MP4 slideshow from a sequence of images.

See [slideshow](./slideshow) for details.

## Utility

#### dump_avc_au

Dump a H.264/AVC file as individual access unit files.

See [dump_avc_au](./dump_avc_au) for details.

#### dump_hevc_au

Split an H.265 (HEVC) elementary stream into access units (AU). Each access unit is saved as a separate file. The sample also displays the NAL units within each access unit.

See [dump_hevc_au](./dump_hevc_au) for details.
