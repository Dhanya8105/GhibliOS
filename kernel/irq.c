
#include "irq.h"
#include "idt.h"
#include "pit.h"

void irq_handler(registers_t *regs) {
    if (regs->int_no >= 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);

    if (interrupt_handlers[regs->int_no])
        interrupt_handlers[regs->int_no](regs);
}
