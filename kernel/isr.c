#include "isr.h"
#include "idt.h"

void isr_handler(registers_t *regs) {
    (void)regs;
    asm volatile("cli; hlt");
}
