#ifndef DRAW_H
#define DRAW_H
#include "framebuffer.h"
#include <stdint.h>

void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_rect_outline(int x, int y, int w, int h, uint32_t color, int t);
void draw_rounded_rect(int x, int y, int w, int h, int r, uint32_t color);
void draw_rounded_rect_outline(int x, int y, int w, int h,
                               int r, uint32_t color, int t);
void draw_circle(int cx, int cy, int r, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_gradient_rect(int x, int y, int w, int h,
                        uint32_t top, uint32_t bottom);
void draw_cloud(int x, int y, int size, uint32_t color);
void draw_sprite_alpha(int x, int y, int w, int h, const uint32_t *px);
void draw_scroll_horizontal(int x, int y, int w, int h, int dx);
#endif
