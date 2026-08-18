#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

/* Ghibli color palette */
#define COLOR_SKY_TOP       0xFF87CEEB
#define COLOR_SKY_HORIZON   0xFFFFF0D6
#define COLOR_CLOUD_WHITE   0xFFFFFAF0
#define COLOR_CLOUD_SHADOW  0xFFE8E8F0
#define COLOR_GRASS_LIGHT   0xFF90EE90
#define COLOR_GRASS_DARK    0xFF228B22
#define COLOR_WINDOW_BG     0xFFFFF8F0
#define COLOR_WINDOW_BORDER 0xFFD2B48C
#define COLOR_ACCENT_PINK   0xFFFFB7C5
#define COLOR_ACCENT_SAGE   0xFF9DC183
#define COLOR_TEXT_DARK     0xFF3D2B1F
#define COLOR_TEXT_MEDIUM   0xFF6B4F3A
#define COLOR_TASKBAR_BG    0xFFFFF8F0
#define COLOR_TRANSPARENT   0x00000000

/* Back buffer — draw here, then call fb_swap() */
extern uint32_t back_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

/* These three must be declared here so kernel.c can call fb_init */
void fb_init(uint32_t addr, uint32_t width, uint32_t height,
             uint32_t pitch, uint8_t bpp);
void fb_swap(void);
void fb_clear(uint32_t color);

/* Inline pixel ops — used everywhere */
static inline void fb_put_pixel(int x, int y, uint32_t color) {
    if ((unsigned)x < SCREEN_WIDTH && (unsigned)y < SCREEN_HEIGHT)
        back_buffer[y][x] = color;
}

static inline uint32_t fb_get_pixel(int x, int y) {
    if ((unsigned)x < SCREEN_WIDTH && (unsigned)y < SCREEN_HEIGHT)
        return back_buffer[y][x];
    return 0;
}

static inline uint32_t fb_blend(uint32_t dst, uint32_t src) {
    uint8_t sa = (src >> 24) & 0xFF;
    if (sa == 0)   return dst;
    if (sa == 255) return src;
    uint8_t da = 255 - sa;
    uint8_t r = (uint8_t)(((src>>16&0xFF)*sa + (dst>>16&0xFF)*da) >> 8);
    uint8_t g = (uint8_t)(((src>> 8&0xFF)*sa + (dst>> 8&0xFF)*da) >> 8);
    uint8_t b = (uint8_t)(((src    &0xFF)*sa + (dst    &0xFF)*da) >> 8);
    return 0xFF000000 | ((uint32_t)r<<16) | ((uint32_t)g<<8) | b;
}

#endif
