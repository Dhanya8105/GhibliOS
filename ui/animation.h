#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>
#define TARGET_FPS    30
#define FRAME_TIME_MS (1000 / TARGET_FPS)
#define MAX_CLOUDS    8
void animation_init(void);
void animation_tick(uint32_t ms);
float animation_float_offset(uint32_t ms, float period, float amplitude);
#endif
