#include "draw.h"
#include "framebuffer.h"
#include <stdint.h>

static uint32_t lerp_color(uint32_t a, uint32_t b, int t, int max) {
    if (max == 0) return a;
    uint8_t ar=(a>>16)&0xFF, ag=(a>>8)&0xFF, ab=a&0xFF;
    uint8_t br=(b>>16)&0xFF, bg=(b>>8)&0xFF, bb=b&0xFF;
    uint8_t r=(uint8_t)(ar+(int)(br-ar)*t/max);
    uint8_t g=(uint8_t)(ag+(int)(bg-ag)*t/max);
    uint8_t bl2=(uint8_t)(ab+(int)(bb-ab)*t/max);
    return 0xFF000000|((uint32_t)r<<16)|((uint32_t)g<<8)|bl2;
}

void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int dy=0;dy<h;dy++)
        for (int dx=0;dx<w;dx++)
            fb_put_pixel(x+dx, y+dy, color);
}

void draw_rect_outline(int x, int y, int w, int h,
                       uint32_t color, int t) {
    draw_rect(x,       y,       w, t, color);
    draw_rect(x,       y+h-t,   w, t, color);
    draw_rect(x,       y,       t, h, color);
    draw_rect(x+w-t,   y,       t, h, color);
}

void draw_rounded_rect(int x, int y, int w, int h,
                       int r, uint32_t color) {
    if (r < 1) { draw_rect(x,y,w,h,color); return; }
    /* Center cross */
    draw_rect(x+r,   y,     w-2*r, h,     color);
    draw_rect(x,     y+r,   r,     h-2*r, color);
    draw_rect(x+w-r, y+r,   r,     h-2*r, color);
    /* Four corners */
    int cx_[4]={x+r,     x+w-r-1, x+r,     x+w-r-1};
    int cy_[4]={y+r,     y+r,     y+h-r-1, y+h-r-1};
    for (int c=0;c<4;c++)
        for (int dy=-r;dy<=r;dy++)
            for (int dx=-r;dx<=r;dx++)
                if (dx*dx+dy*dy<=r*r)
                    fb_put_pixel(cx_[c]+dx, cy_[c]+dy, color);
}

void draw_rounded_rect_outline(int x, int y, int w, int h,
                               int r, uint32_t color, int t) {
    draw_rounded_rect(x,   y,   w,     h,     r,       color);
    uint32_t inner = fb_get_pixel(x+r+t, y+t);
    if (w>2*t && h>2*t)
        draw_rounded_rect(x+t, y+t, w-2*t, h-2*t,
                          r>t?r-t:0, inner);
}

void draw_circle(int cx, int cy, int r, uint32_t color) {
    for (int dy=-r;dy<=r;dy++)
        for (int dx=-r;dx<=r;dx++)
            if (dx*dx+dy*dy<=r*r)
                fb_put_pixel(cx+dx, cy+dy, color);
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx=x1-x0; if(dx<0)dx=-dx;
    int dy=y1-y0; if(dy<0)dy=-dy;
    int sx=x0<x1?1:-1, sy=y0<y1?1:-1, err=dx-dy;
    while(1){
        fb_put_pixel(x0,y0,color);
        if(x0==x1&&y0==y1)break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x0+=sx;}
        if(e2< dx){err+=dx;y0+=sy;}
    }
}

void draw_gradient_rect(int x, int y, int w, int h,
                        uint32_t top, uint32_t bot) {
    for (int dy=0;dy<h;dy++){
        uint32_t col=lerp_color(top,bot,dy,h-1);
        for (int dx=0;dx<w;dx++)
            fb_put_pixel(x+dx,y+dy,col);
    }
}

void draw_cloud(int x, int y, int size, uint32_t color) {
    int r=size/2;
    draw_circle(x,       y,   r,     color);
    draw_circle(x+r,     y,   r,     color);
    draw_circle(x+r*2,   y,   r,     color);
    draw_circle(x+r/2,   y-r/2, r*2/3, color);
    draw_circle(x+r*3/2, y-r/2, r*4/5, color);
    draw_rect(x, y, r*3, r, color);
}

void draw_sprite_alpha(int x, int y, int w, int h,
                       const uint32_t *px) {
    for (int dy=0;dy<h;dy++)
        for (int dx=0;dx<w;dx++){
            uint32_t src=px[dy*w+dx];
            if((src>>24)==0) continue;
            uint32_t dst=fb_get_pixel(x+dx,y+dy);
            fb_put_pixel(x+dx,y+dy,fb_blend(dst,src));
        }
}

void draw_scroll_horizontal(int x, int y, int w, int h, int dx) {
    if(dx<0)
        for(int row=y;row<y+h;row++)
            for(int col=x;col<x+w+dx;col++)
                fb_put_pixel(col,row,fb_get_pixel(col-dx,row));
    else
        for(int row=y;row<y+h;row++)
            for(int col=x+w-1;col>=x+dx;col--)
                fb_put_pixel(col,row,fb_get_pixel(col-dx,row));
}
