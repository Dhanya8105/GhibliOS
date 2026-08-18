	#ifndef FONT_H
#define FONT_H
#include <stdint.h>
#define FONT_WIDTH  8
#define FONT_HEIGHT 8
void font_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void font_draw_string(int x, int y, const char *s, uint32_t fg, uint32_t bg);
#endif

