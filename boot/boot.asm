; ============================================================
; GhibliOS — boot/boot.asm
; Complete multiboot bootloader with framebuffer request
; ============================================================

; ---- Multiboot constants ----
MAGIC    equ 0x1BADB002
FLAGS    equ (1<<0) | (1<<1) | (1<<2)
CHECKSUM equ -(MAGIC + FLAGS)

; ---- Multiboot header section ----
; GRUB scans the first 8KB for the magic number.
; We put it in its own section and force it first via linker.ld
section .multiboot
align 4
    dd MAGIC        ; magic number
    dd FLAGS        ; feature flags
    dd CHECKSUM     ; checksum: magic+flags+checksum must = 0

    ; Address fields — all zero means "use ELF headers"
    dd 0            ; header_addr
    dd 0            ; load_addr
    dd 0            ; load_end_addr
    dd 0            ; bss_end_addr
    dd 0            ; entry_addr

    ; Video mode request — GRUB sets up framebuffer for us
    dd 0            ; mode_type: 0 = linear framebuffer
    dd 800          ; width
    dd 600          ; height
    dd 32           ; depth (bits per pixel)

; ---- Stack ----
; C needs a stack before it can do anything.
; The stack grows downward so we point ESP at the top.
section .bss
align 16
stack_bottom:
    resb 16384      ; 16KB stack
stack_top:

; ---- Kernel entry point ----
section .text
global start
extern kernel_main

start:
    ; Disable interrupts until IDT is set up
    cli

    ; Set up the stack pointer
    mov esp, stack_top

    ; GRUB passes us two values:
    ;   EAX = multiboot magic (0x2BADB002)
    ;   EBX = pointer to multiboot_info_t struct
    ; Push them as arguments to kernel_main(uint32_t magic, mb_info_t *mbi)
    push ebx        ; arg2: multiboot info pointer
    push eax        ; arg1: magic number

    ; Call into C
    call kernel_main

    ; kernel_main should never return.
    ; If it does, hang the CPU safely.
    cli
.hang:
    hlt
    jmp .hang

; ============================================================
; GDT flush — called from kernel/gdt.c
; Loads the GDT and reloads all segment registers
; ============================================================
global gdt_flush
gdt_flush:
    mov eax, [esp+4]    ; get gdt_ptr argument
    lgdt [eax]          ; load GDT register

    ; Far jump to reload the code segment register (CS)
    ; 0x08 = first GDT entry = kernel code segment
    jmp 0x08:.reload_cs
.reload_cs:
    ; Reload all data segment registers
    ; 0x10 = second GDT entry = kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; ============================================================
; IDT flush — called from kernel/idt.c
; Loads the IDT pointer into the CPU's IDTR register
; ============================================================
global idt_flush
idt_flush:
    mov eax, [esp+4]    ; get idt_ptr argument
    lidt [eax]          ; load IDT register
    ret

; ============================================================
; IRQ stubs — hardware interrupt entry points
;
; When an IRQ fires the CPU needs a known entry point.
; Each stub:
;   1. Pushes a dummy error code (0) for uniformity
;   2. Pushes the interrupt number (32 + IRQ number)
;   3. Jumps to irq_common which saves all registers
;      and calls irq_handler() in kernel/irq.c
; ============================================================

%macro IRQ_STUB 1
global irq%1
irq%1:
    cli
    push dword 0            ; dummy error code
    push dword (32 + %1)    ; interrupt number
    jmp irq_common
%endmacro

; Define stubs for IRQ 0-15
IRQ_STUB 0    ; IRQ0  = PIT timer      → interrupt 32
IRQ_STUB 1    ; IRQ1  = keyboard       → interrupt 33
IRQ_STUB 2    ; IRQ2  = cascade        → interrupt 34
IRQ_STUB 3    ; IRQ3  = COM2           → interrupt 35
IRQ_STUB 4    ; IRQ4  = COM1           → interrupt 36
IRQ_STUB 5    ; IRQ5  = LPT2           → interrupt 37
IRQ_STUB 6    ; IRQ6  = floppy         → interrupt 38
IRQ_STUB 7    ; IRQ7  = LPT1           → interrupt 39
IRQ_STUB 8    ; IRQ8  = RTC            → interrupt 40
IRQ_STUB 9    ; IRQ9  = free           → interrupt 41
IRQ_STUB 10   ; IRQ10 = free           → interrupt 42
IRQ_STUB 11   ; IRQ11 = free           → interrupt 43
IRQ_STUB 12   ; IRQ12 = PS/2 mouse     → interrupt 44
IRQ_STUB 13   ; IRQ13 = FPU            → interrupt 45
IRQ_STUB 14   ; IRQ14 = ATA primary    → interrupt 46
IRQ_STUB 15   ; IRQ15 = ATA secondary  → interrupt 47

; ---- Common IRQ handler ----
extern irq_handler

irq_common:
    ; Save all general purpose registers
    pusha

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment into all segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    ; Push stack pointer as argument (this IS the registers_t struct)
    push esp

    ; Call the C handler
    call irq_handler

    ; Clean up the argument we pushed
    pop eax

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general purpose registers
    popa

    ; Clean up the two dwords pushed by the stub
    ; (error code + interrupt number)
    add esp, 8

    ; Re-enable interrupts and return from interrupt
    sti
    iret
