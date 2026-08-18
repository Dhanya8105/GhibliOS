#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "../drivers/keyboard.h"
#include "../drivers/speaker.h"
#include "../graphics/framebuffer.h"
#include "../graphics/draw.h"
#include "../graphics/font.h"
#include "../ui/animation.h"
#include "../ui/shell.h"
#include "../drivers/pci.h"
#include "../drivers/ac97.h"
#include "../drivers/audio_data.h"
/* VGA text buffer — always at 0xB8000, always works */
static void vga_print(const char *s) {
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    static int pos = 0;
    while (*s) {
        vga[pos++] = (0x0F << 8) | (uint8_t)*s;
        s++;
    }
}

/* Multiboot info — we only need the framebuffer fields */
typedef struct {
    uint32_t flags;
    uint32_t mem_lower, mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count, mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length, mmap_addr;
    uint32_t drives_length, drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed)) mb_info_t;

void kernel_main(uint32_t magic, mb_info_t *mbi) {
    (void)magic;

    /* Step 1: tell us we're alive via VGA text */
    vga_print("GhibliOS kernel alive!");

    /* Step 2: CPU init */
    gdt_init();
    vga_print(" GDT ok.");

    idt_init();
    vga_print(" IDT ok.");

    pit_init();
    vga_print(" PIT ok.");

    keyboard_init();
    vga_print(" KB ok.");

    /* Step 3: find framebuffer */
    uint32_t fb_addr  = 0xFD000000;
    uint32_t fb_w     = 800;
    uint32_t fb_h     = 600;
    uint32_t fb_pitch = 800 * 4;
    uint8_t  fb_bpp   = 32;

    /* Check if GRUB gave us framebuffer info (flag bit 12) */
    if (mbi && (mbi->flags & (1 << 12))) {
        fb_addr  = (uint32_t)(mbi->framebuffer_addr & 0xFFFFFFFF);
        fb_w     = mbi->framebuffer_width;
        fb_h     = mbi->framebuffer_height;
        fb_pitch = mbi->framebuffer_pitch;
        fb_bpp   = mbi->framebuffer_bpp;
        vga_print(" FB:GRUB.");
    } else {
        vga_print(" FB:probe.");
    }

    /* Step 4: init framebuffer */
    fb_init(fb_addr, fb_w, fb_h, fb_pitch, fb_bpp);
    vga_print(" fbinit ok.");

    /* Step 5: draw a test pixel — prove fb works */
    fb_clear(0xFF87CEEB);   /* sky blue */
    fb_swap();
    vga_print(" draw ok.");

    /* Step 6: full UI */
    animation_init();
    shell_init();
    if (ac97_init()) {
        vga_print(" AC97 ok.");
        ac97_play_loop(theme_audio_start, theme_audio_size);
        vga_print(" music started.");
    } else {
        vga_print(" AC97 not found.");
    }

    speaker_chime();

   
   while (1) {
        animation_tick(pit_get_ticks());
        speaker_service();
        asm volatile("hlt");
    }
}
