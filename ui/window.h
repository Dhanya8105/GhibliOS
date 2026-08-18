#ifndef WINDOW_H
#define WINDOW_H
#include <stdint.h>
int  window_create(int x, int y, int w, int h,
                   const char *title, int floats);
void ui_draw(uint32_t ms);
#endif
