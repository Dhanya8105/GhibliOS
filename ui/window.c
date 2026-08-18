
#include "window.h"
#include "cursor.h"
#include "shell.h"
#include "animation.h"
#include "../graphics/draw.h"
#include "../graphics/font.h"
#include "../graphics/framebuffer.h"
#include <stdint.h>

#define MAX_WINDOWS 4

extern uint8_t g_last_scancode;

typedef struct {
    int x, y, w, h;
    char title[32];
    int  visible;
    int  floats;
    uint32_t phase;
} win_t;

static win_t wins[MAX_WINDOWS];
static int   win_count = 0;

int window_create(int x, int y, int w, int h,
                  const char *title, int floats) {
    if (win_count >= MAX_WINDOWS) return -1;
    int id = win_count++;
    wins[id].x = x; wins[id].y = y;
    wins[id].w = w; wins[id].h = h;
    wins[id].visible = 1;
    wins[id].floats  = floats;
    wins[id].phase   = (uint32_t)(id * 800);
    int i = 0;
    while (title[i] && i < 31) {
        wins[id].title[i] = title[i]; i++;
    }
    wins[id].title[i] = '\0';
    return id;
}

static void win_draw(win_t *w, uint32_t ms) {
    if (!w->visible) return;
    int fy = 0;
    if (w->floats)
        fy = (int)animation_float_offset(ms + w->phase, 4000.0f, 4.0f);

    int x = w->x, y = w->y + fy;

    /* Shadow */
    draw_rounded_rect(x+4, y+4, w->w, w->h, 12, 0x44A08060);
    /* Body */
    draw_rounded_rect(x, y, w->w, w->h, 12, COLOR_WINDOW_BG);
    /* Border */
    draw_rect_outline(x, y, w->w, w->h, COLOR_WINDOW_BORDER, 2);
    /* Title bar */
    draw_rect(x+2, y+2, w->w-4, 22, 0xFFEEDDCC);
    /* Title text */
    font_draw_string(x+10, y+7, w->title,
                     COLOR_TEXT_DARK, COLOR_TRANSPARENT);
    /* Close dot */
    draw_circle(x + w->w - 12, y + 13, 5, COLOR_ACCENT_PINK);

    /* Shell content inside terminal window */
    shell_draw();
}

static void taskbar_draw(uint32_t ms) {
    (void)ms;
    int ty = SCREEN_HEIGHT - 38;
    draw_rect(0, ty, SCREEN_WIDTH, 38, COLOR_TASKBAR_BG);
    draw_rect(0, ty, SCREEN_WIDTH, 1,  COLOR_WINDOW_BORDER);
    font_draw_string(10, ty+13, "GhibliOS",
                     COLOR_TEXT_DARK, COLOR_TRANSPARENT);
    font_draw_string(SCREEN_WIDTH-56, ty+13, "12:00",
                     COLOR_TEXT_DARK, COLOR_TRANSPARENT);
    draw_rounded_rect(SCREEN_WIDTH/2-35, ty+5, 70, 26, 6,
                      COLOR_ACCENT_SAGE);
    font_draw_string(SCREEN_WIDTH/2-14, ty+13, "Apps",
                     0xFF2D4A1F, COLOR_TRANSPARENT);

    /* Debug: last raw scancode received */
    char dbg[8];
    dbg[0]='0'+((g_last_scancode/100)%10);
    dbg[1]='0'+((g_last_scancode/10)%10);
    dbg[2]='0'+(g_last_scancode%10);
    dbg[3]='\0';
    font_draw_string(SCREEN_WIDTH-140, ty+13, dbg,
                     COLOR_TEXT_DARK, COLOR_TRANSPARENT);
}

void ui_draw(uint32_t ms) {
    for (int i = 0; i < win_count; i++)
        win_draw(&wins[i], ms);
    taskbar_draw(ms);
    cursor_draw(ms);
}

