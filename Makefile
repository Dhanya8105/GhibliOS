# ============================================================
# GhibliOS Makefile — macOS compatible
# ============================================================

CC      := i686-elf-gcc
AS      := nasm
LD      := i686-elf-gcc

CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra \
           -fno-stack-protector -fno-pic -m32

LDFLAGS := -ffreestanding -O2 -nostdlib -m32

ASFLAGS := -f elf32

BOOT_OBJ  := boot/boot.o assets/theme_audio.o

C_SOURCES := kernel/kernel.c \
             kernel/gdt.c    \
             kernel/idt.c    \
             kernel/isr.c    \
             kernel/irq.c    \
             kernel/pit.c    \
             drivers/keyboard.c \
             drivers/speaker.c  \
	     drivers/pci.c      \
             drivers/ac97.c     \
             graphics/framebuffer.c \
             graphics/draw.c    \
             graphics/font.c    \
             ui/window.c   \
             ui/cursor.c   \
             ui/theme.c    \
             ui/animation.c \
	     ui/spirits.c  \
             ui/shell.c

C_OBJS   := $(C_SOURCES:.c=.o)
ALL_OBJS := $(BOOT_OBJ) $(C_OBJS)

# ---- Targets ----

all: GhibliOS.iso

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

assets/theme_audio.o: assets/theme_audio.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

GhibliOS.bin: $(ALL_OBJS)
	$(LD) -T linker.ld -o $@ $(LDFLAGS) $(ALL_OBJS) -lgcc

GhibliOS.iso: GhibliOS.bin
	mkdir -p iso/boot/grub
	cp GhibliOS.bin iso/boot/
	cp grub.cfg     iso/boot/grub/grub.cfg
	i686-elf-grub-mkrescue -o GhibliOS.iso iso


run: GhibliOS.iso
	qemu-system-i386 \
	-cdrom GhibliOS.iso \
	-m 256M \
	-vga std \
	-no-reboot \
	-no-shutdown \
	-audiodev coreaudio,id=snd0,out.frequency=44100,out.channels=2,out.format=s16 \
	-device AC97,audiodev=snd0

debug: GhibliOS.iso
	qemu-system-i386 \
	-cdrom GhibliOS.iso \
	-m 256M \
	-vga std \
	-no-reboot \
	-no-shutdown \
	-d int,cpu_reset \
	-D qemu.log

clean:
	rm -rf iso/ GhibliOS.bin GhibliOS.iso qemu.log
	find . -name "*.o" -delete

.PHONY: all run debug clean
