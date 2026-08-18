#include "cursor.h"
#include "../graphics/draw.h"
#include "../graphics/framebuffer.h"
#include <stdint.h>

static int cx = SCREEN_WIDTH/2;
static int cy = SCREEN_HEIGHT/2;

void cursor_draw(uint32_t ms) {
    (void)ms;
    /* Arrow: series of horizontal lines narrowing downward */
    for (int row = 0; row < 14; row++) {
        int w = 12 - row;
        if (w < 1) w = 1;
        draw_rect(cx, cy+row, w, 1, 0xCCC8B08C);
    }
    /* White inner */
    for (int row = 1; row < 13; row++) {
        int w = 10 - row;
        if (w < 1) break;
        draw_rect(cx+1, cy+row, w, 1, 0xFFFFF8F0);
    }
    draw_circle(cx, cy, 2, COLOR_TEXT_DARK);
}

void cursor_move(int dx, int dy) {
    cx += dx; cy += dy;
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    if (cx >= SCREEN_WIDTH)  cx = SCREEN_WIDTH-1;
    if (cy >= SCREEN_HEIGHT) cy = SCREEN_HEIGHT-1;
}
