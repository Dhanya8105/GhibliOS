#include "framebuffer.h"
#include <stdint.h>

uint32_t back_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

static volatile uint32_t *video_mem    = 0;
static uint32_t           pitch_words  = 0;

/* Test if an address is writable memory (not a crash) */
static int addr_writable(uint32_t addr) {
    volatile uint32_t *p = (volatile uint32_t *)addr;
    uint32_t old = *p;
    *p = 0x12345678;
    uint32_t got = *p;
    *p = old;
    return got == 0x12345678;
}

/* Scan known QEMU VBE framebuffer addresses */
static uint32_t probe_fb(void) {
    uint32_t addrs[] = {
        0xFD000000,
        0xE0000000,
        0xF0000000,
        0xFC000000,
        0xFE000000,
        0xC0000000,
        0
    };
    for (int i = 0; addrs[i]; i++)
        if (addr_writable(addrs[i]))
            return addrs[i];
    return 0xFD000000;
}

void fb_init(uint32_t addr, uint32_t width, uint32_t height,
             uint32_t pitch, uint8_t bpp) {
    (void)width; (void)height; (void)bpp;

    /* If address looks wrong, probe for it */
    if (addr == 0xFD000000 && !addr_writable(addr))
        addr = probe_fb();

    video_mem   = (volatile uint32_t *)addr;
    pitch_words = pitch / 4;

    fb_clear(COLOR_SKY_TOP);
}

void fb_clear(uint32_t color) {
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            back_buffer[y][x] = color;
}

void fb_swap(void) {
    if (!video_mem) return;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        volatile uint32_t *row = video_mem + (uint32_t)y * pitch_words;
        for (int x = 0; x < SCREEN_WIDTH; x++)
            row[x] = back_buffer[y][x];
    }
}
