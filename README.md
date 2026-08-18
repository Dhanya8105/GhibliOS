# GhibliOS
## Building with audio

The background music file (`assets/theme_stereo16.pcm`) is excluded from
this repo since it's derived from copyrighted audio and shouldn't be
redistributed. To build with music:

1. Place your own audio source (e.g. `theme.mp3`) somewhere accessible.
2. Convert it to the required raw PCM format using ffmpeg:

   ffmpeg -i theme.mp3 -ss 00:00:49 -to 00:01:29 -ar 22050 -ac 2 \
     -f s16le -acodec pcm_s16le assets/theme_stereo16.pcm

3. Run `make clean && make run` as usual.

Without this file, the build will fail at the `incbin` step in
`assets/theme_audio.asm` — either provide a PCM file at that path or
comment out the `incbin` line to build a silent version.
