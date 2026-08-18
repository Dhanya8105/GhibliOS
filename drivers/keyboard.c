
#include "keyboard.h"
#include "../kernel/idt.h"
#include "../kernel/pit.h"
#include "../ui/shell.h"

uint8_t g_last_scancode = 0;

static const char sc_low[] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

static const char sc_hi[] = {
    0,0,'!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' '
};

static int shift = 0;

static void kb_handler(registers_t *regs) {
    (void)regs;
    uint8_t sc = inb(0x60);
    if (sc & 0x80) {
        if ((sc & 0x7F) == 0x2A || (sc & 0x7F) == 0x36) shift = 0;
        return;
    }
    if (sc == 0x2A || sc == 0x36) { shift = 1; return; }

    g_last_scancode = sc;

    if (sc >= sizeof(sc_low)) return;
    char c = shift ? sc_hi[sc] : sc_low[sc];
    if (c) shell_keypress(c);
}

void keyboard_init(void) {
    register_interrupt_handler(IRQ1, kb_handler);
}

