#include "pit.h"
#include "idt.h"
#include <stdint.h>

static volatile uint32_t ticks = 0;

static void pit_handler(registers_t *regs) {
    (void)regs;
    ticks++;
}

void pit_init(void) {
    uint32_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
    register_interrupt_handler(IRQ0, pit_handler);
}

uint32_t pit_get_ticks(void) {
    return ticks;
}

void pit_sleep(uint32_t ms) {
    uint32_t target = ticks + ms;
    while (ticks < target)
        asm volatile("hlt");
}
