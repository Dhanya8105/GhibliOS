#include "spirits.h"
#include "../graphics/draw.h"
#include "../graphics/framebuffer.h"
#include <stdint.h>

#define MAX_SPIRITS 5

typedef enum { SPIRIT_FOREST, SPIRIT_LANTERN, SPIRIT_CAT } spirit_kind_t;

typedef struct {
    int x, y, size, speed;
    spirit_kind_t kind;
    uint32_t color;
} spirit_t;

static spirit_t spirits[MAX_SPIRITS];

static uint32_t rng(uint32_t *s) {
    *s = *s * 1103515245 + 12345;
    return (*s >> 16) & 0x7FFF;
}

void spirits_init(void) {
    uint32_t s = 777;
    for (int i = 0; i < MAX_SPIRITS; i++) {
        spirits[i].x     = (int)(rng(&s) % SCREEN_WIDTH);
        spirits[i].y     = 60 + (int)(rng(&s) % 250);
        spirits[i].size  = 14 + (int)(rng(&s) % 10);
        spirits[i].speed = 1 + (int)(rng(&s) % 2);
        spirits[i].kind  = (spirit_kind_t)(i % 3);

        switch (spirits[i].kind) {
            case SPIRIT_FOREST:  spirits[i].color = 0xFFEFE7D0; break;
            case SPIRIT_LANTERN: spirits[i].color = 0xFFFFD59A; break;
            case SPIRIT_CAT:     spirits[i].color = 0xFFD9D2E9; break;
        }
    }
}

void spirits_update(void) {
    uint32_t s = 321;
    for (int i = 0; i < MAX_SPIRITS; i++) {
        spirits[i].x -= spirits[i].speed;
        if (spirits[i].x + spirits[i].size * 2 < 0) {
            spirits[i].x = SCREEN_WIDTH + 20;
            spirits[i].y = 60 + (int)(rng(&s) % 250);
        }
    }
}

static void draw_forest_spirit(int x, int y, int size, uint32_t color) {
    int r = size / 2;
    draw_circle(x, y, r, color);              /* body */
    draw_circle(x - r/2, y + r/3, r/4, color); /* left arm */
    draw_circle(x + r/2, y + r/3, r/4, color); /* right arm */
    draw_circle(x - r/3, y - r/5, 2, 0xFF3A2E22); /* left eye */
    draw_circle(x + r/3, y - r/5, 2, 0xFF3A2E22); /* right eye */
}

static void draw_lantern_spirit(int x, int y, int size, uint32_t color) {
    int r = size / 2;
    draw_circle(x, y, r, color);               /* glowing orb */
    draw_circle(x, y, r + 2, (color & 0x00FFFFFF) | 0x33000000); /* soft glow ring */
    /* ribbon tail */
    draw_line(x - 2, y + r, x - 4, y + r + 6, 0xFFFFD59A);
    draw_line(x + 2, y + r, x + 4, y + r + 8, 0xFFFFD59A);
    draw_line(x,     y + r, x,     y + r + 5, 0xFFFFD59A);
}

static void draw_cloud_cat(int x, int y, int size, uint32_t color) {
    int r = size / 2;
    draw_circle(x, y, r, color);                       /* body */
    draw_circle(x - r/2, y - r/2, r/3, color);          /* left ear */
    draw_circle(x + r/2, y - r/2, r/3, color);          /* right ear */
    draw_circle(x - r/3, y - r/6, 1, 0xFF3A2E22);       /* left eye */
    draw_circle(x + r/3, y - r/6, 1, 0xFF3A2E22);       /* right eye */
}

void spirits_draw(void) {
    for (int i = 0; i < MAX_SPIRITS; i++) {
        spirit_t *sp = &spirits[i];
        switch (sp->kind) {
            case SPIRIT_FOREST:  draw_forest_spirit(sp->x, sp->y, sp->size, sp->color); break;
            case SPIRIT_LANTERN: draw_lantern_spirit(sp->x, sp->y, sp->size, sp->color); break;
            case SPIRIT_CAT:     draw_cloud_cat(sp->x, sp->y, sp->size, sp->color); break;
        }
    }
}
