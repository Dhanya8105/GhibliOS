#include "shell.h"
#include "window.h"
#include "../graphics/draw.h"
#include "../graphics/font.h"
#include "../graphics/framebuffer.h"
#include "../drivers/speaker.h"
#include "../kernel/pit.h"
#include <stdint.h>

#define WIN_X  40
#define WIN_Y  70
#define WIN_W  500
#define WIN_H  340
#define PAD    12
#define LH     (FONT_HEIGHT + 3)
#define MAXL   20
#define MAXC   58

static char lines[MAXL][MAXC];
static int  lcount = 0;
static char ibuf[MAXC];
static int  ilen  = 0;

/* --- tiny in-memory filesystem --- */
typedef struct { const char *name; const char *content; } fake_file_t;
static const fake_file_t files[] = {
    {"about.txt",   "GhibliOS - a hand-built bare metal OS."},
    {"readme.txt",  "Type help to see available commands."},
    {"notes.txt",   "Built with love, C, and a lot of debugging."}
};
#define FILE_COUNT (int)(sizeof(files)/sizeof(files[0]))

static int kstrcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

static void puts_line(const char *s) {
    if (lcount >= MAXL) {
        for (int i = 0; i < MAXL-1; i++) {
            int j = 0;
            while ((lines[i][j] = lines[i+1][j])) j++;
        }
        lcount = MAXL-1;
    }
    int j = 0;
    while (*s && j < MAXC-1) lines[lcount][j++] = *s++;
    lines[lcount][j] = '\0';
    lcount++;
}

/* Splits "cmd arg1 arg2" into cmd + args pointer. Mutates the buffer. */
static char* split_args(char *cmd) {
    while (*cmd && *cmd != ' ') cmd++;
    if (*cmd == ' ') {
        *cmd = '\0';
        return cmd + 1;
    }
    return cmd; /* points at trailing '\0', i.e. no args */
}

static void print_uint(uint32_t v) {
    char buf[12];
    int i = 0;
    if (v == 0) { puts_line("0"); return; }
    while (v > 0 && i < 11) { buf[i++] = '0' + (v % 10); v /= 10; }
    char out[12];
    int j = 0;
    while (i > 0) out[j++] = buf[--i];
    out[j] = '\0';
    puts_line(out);
}

static void exec_cmd(char *raw) {
    speaker_ui_click();

    char *args = split_args(raw);
    const char *cmd = raw;

    if (cmd[0] == '\0') return;

    if (kstrcmp(cmd, "help") == 0) {
        puts_line("Commands:");
        puts_line("help clear about chime hi echo time ver ls cat");
    }
    else if (kstrcmp(cmd, "clear") == 0) {
        lcount = 0;
    }
    else if (kstrcmp(cmd, "about") == 0) {
        puts_line("GhibliOS v0.1");
        puts_line("Ghibli-inspired OS in C.");
    }
    else if (kstrcmp(cmd, "chime") == 0) {
        puts_line("*plays chime*");
        speaker_chime();
    }
    else if (kstrcmp(cmd, "hi") == 0) {
        puts_line("Hello, traveller! ~");
    }
    else if (kstrcmp(cmd, "ver") == 0) {
        puts_line("GhibliOS kernel v0.1 (i686, bare metal)");
    }
    else if (kstrcmp(cmd, "time") == 0) {
        puts_line("Ticks since boot:");
        print_uint(pit_get_ticks());
    }
    else if (kstrcmp(cmd, "echo") == 0) {
        if (args[0] == '\0') puts_line("");
        else puts_line(args);
    }
    else if (kstrcmp(cmd, "ls") == 0) {
        for (int i = 0; i < FILE_COUNT; i++)
            puts_line(files[i].name);
    }
    else if (kstrcmp(cmd, "cat") == 0) {
        if (args[0] == '\0') {
            puts_line("Usage: cat <filename>");
        } else {
            int found = 0;
            for (int i = 0; i < FILE_COUNT; i++) {
                if (kstrcmp(args, files[i].name) == 0) {
                    puts_line(files[i].content);
                    found = 1;
                    break;
                }
            }
            if (!found) puts_line("cat: file not found");
        }
    }
    else {
        puts_line("Unknown. Type 'help'.");
    }
}

void shell_init(void) {
    window_create(WIN_X, WIN_Y, WIN_W, WIN_H, "Terminal", 1);
    puts_line("Welcome to GhibliOS ~");
    puts_line("Type 'help' to begin.");
}

void shell_keypress(char c) {
    if (c == '\n') {
        char echo[MAXC+3];
        echo[0]='>'; echo[1]=' ';
        int i;
        for (i=0;i<ilen;i++) echo[i+2]=ibuf[i];
        echo[i+2]='\0';
        puts_line(echo);

        ibuf[ilen]='\0';
        exec_cmd(ibuf);

        ilen=0;
        ibuf[0]='\0';
    } else if (c=='\b') {
        if (ilen>0) { ilen--; ibuf[ilen] = '\0'; }
    } else {
        if (ilen<MAXC-1) { ibuf[ilen++]=c; ibuf[ilen]='\0'; }
    }
}

void shell_draw(void) {
    int bx = WIN_X + PAD;
    int by = WIN_Y + 30 + PAD;
    for (int i=0;i<lcount;i++)
        font_draw_string(bx, by+i*LH, lines[i],
                         COLOR_TEXT_DARK, COLOR_TRANSPARENT);
    int iy = by + lcount*LH;
    font_draw_string(bx, iy, "> ", COLOR_TEXT_MEDIUM, COLOR_TRANSPARENT);
    font_draw_string(bx+16, iy, ibuf, COLOR_TEXT_DARK, COLOR_TRANSPARENT);
    draw_rect(bx+16+ilen*(FONT_WIDTH+1), iy,
              FONT_WIDTH, FONT_HEIGHT, COLOR_ACCENT_PINK);
}
