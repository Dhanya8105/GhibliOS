section .rodata
align 4

global theme_audio_start
global theme_audio_end
global theme_audio_size

theme_audio_start:
    incbin "assets/theme_stereo16.pcm"
theme_audio_end:

theme_audio_size:
    dd theme_audio_end - theme_audio_start
