#ifndef IRQ_H
#define IRQ_H
#include "idt.h"
void irq_handler(registers_t *regs);
#endif
