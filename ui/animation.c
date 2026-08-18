#include "spirits.h"
#include "animation.h"
#include "window.h"
#include "shell.h"
#include "../graphics/draw.h"
#include "../graphics/framebuffer.h"
#include "../kernel/pit.h"
#include <stdint.h>

/* ---- Cloud state ---- */
typedef struct {
    int x, y, size, speed;
    uint32_t color;
} cloud_t;

static cloud_t clouds[MAX_CLOUDS];
static uint32_t last_ms = 0;

static uint32_t rng(uint32_t *s) {
    *s = *s * 1103515245 + 12345;
    return (*s >> 16) & 0x7FFF;
}

static void clouds_init(void) {
    uint32_t s = 42;
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].x     = (int)(rng(&s) % SCREEN_WIDTH);
        clouds[i].y     = 20 + (int)(rng(&s) % 130);
        clouds[i].size  = 25 + (int)(rng(&s) % 35);
        clouds[i].speed = 1 + (int)(rng(&s) % 2);
        clouds[i].color = (i % 3 == 0)
                          ? COLOR_CLOUD_SHADOW
                          : COLOR_CLOUD_WHITE;
    }
}

static void clouds_update(void) {
    uint32_t s = 99;
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].x -= clouds[i].speed;
        if (clouds[i].x + clouds[i].size * 3 < 0) {
            clouds[i].x = SCREEN_WIDTH + 20;
            clouds[i].y = 20 + (int)(rng(&s) % 130);
        }
    }
}

static void sky_draw(void) {
    draw_gradient_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 80,
                       COLOR_SKY_TOP, COLOR_SKY_HORIZON);
    draw_gradient_rect(0, SCREEN_HEIGHT - 80, SCREEN_WIDTH, 80,
                       COLOR_GRASS_LIGHT, COLOR_GRASS_DARK);
}

void animation_init(void) {
    clouds_init();
    spirits_init();
    last_ms = pit_get_ticks();
}

void animation_tick(uint32_t ms) {
    if (ms - last_ms < FRAME_TIME_MS) return;
    last_ms = ms;

    sky_draw();

    clouds_update();
    for (int i = 0; i < MAX_CLOUDS; i++)
        draw_cloud(clouds[i].x, clouds[i].y,
                   clouds[i].size, clouds[i].color);
    spirits_update();
    spirits_draw();

    ui_draw(ms);
    fb_swap();
}

float animation_float_offset(uint32_t ms, float period, float amplitude) {
    /* Simple sine approximation using integer math */
    uint32_t phase = (uint32_t)(ms * 256 / (uint32_t)period) & 0xFF;
    /* sin table scaled to -100..100 */
    static const int8_t t[256] = {
        0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,
        48,51,54,57,59,62,65,67,70,73,75,78,80,82,85,87,
        89,91,93,95,97,98,100,101,103,104,105,106,107,108,
        108,109,109,109,110,109,109,109,108,108,107,106,105,
        104,103,101,100,98,97,95,93,91,89,87,85,82,80,78,75,
        73,70,67,65,62,59,57,54,51,48,45,42,39,36,33,30,27,
        24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,
        -27,-30,-33,-36,-39,-42,-45,-48,-51,-54,-57,-59,-62,
        -65,-67,-70,-73,-75,-78,-80,-82,-85,-87,-89,-91,-93,
        -95,-97,-98,-100,-101,-103,-104,-105,-106,-107,-108,
        -108,-109,-109,-109,-110,-109,-109,-109,-108,-108,
        -107,-106,-105,-104,-103,-101,-100,-98,-97,-95,-93,
        -91,-89,-87,-85,-82,-80,-78,-75,-73,-70,-67,-65,-62,
        -59,-57,-54,-51,-48,-45,-42,-39,-36,-33,-30,-27,-24,
        -21,-18,-15,-12,-9,-6,-3
    };
    float v = (float)t[phase] / 100.0f;
    return v * amplitude;
}
