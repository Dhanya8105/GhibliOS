
#include "idt.h"
#include "pit.h"
#include <stdint.h>

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

isr_t interrupt_handlers[256];

extern void idt_flush(uint32_t);
extern void irq0(void);
extern void irq1(void);

static void idt_set(uint8_t n, uint32_t base) {
    idt[n].base_lo = base & 0xFFFF;
    idt[n].base_hi = (base >> 16) & 0xFFFF;
    idt[n].sel     = 0x08;
    idt[n].zero    = 0;
    idt[n].flags   = 0x8E;
}

void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set(i, 0);
        interrupt_handlers[i] = 0;
    }

    /* Remap PIC */
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x00); outb(0xA1, 0x00);

    idt_set(32, (uint32_t)irq0);
    idt_set(33, (uint32_t)irq1);

    idt_flush((uint32_t)&idtp);
    asm volatile("sti");
}

void register_interrupt_handler(uint8_t n, isr_t h) {
    interrupt_handlers[n] = h;
}
