
#include "speaker.h"
#include "../kernel/pit.h"

static uint32_t pending_freq = 0;
static uint32_t pending_ms   = 0;

static void speaker_start(uint32_t freq) {
    uint32_t div = 1193182 / freq;
    outb(0x43, 0xB6);
    outb(0x42, div & 0xFF);
    outb(0x42, (div >> 8) & 0xFF);
    uint8_t tmp = inb(0x61);
    outb(0x61, tmp | 0x03);
}

void speaker_stop(void) {
    outb(0x61, inb(0x61) & ~0x03);
}

/* Non-blocking: just records a request. Safe to call from interrupts. */
void speaker_request(uint32_t freq, uint32_t ms) {
    pending_freq = freq;
    pending_ms   = ms;
}

void speaker_ui_click(void) {
    speaker_request(800, 15);
}

void speaker_chime(void) {
    speaker_request(1047, 70);
}

/* Blocking: only ever call from the main loop, never from an interrupt. */
void speaker_service(void) {
    if (pending_freq) {
        speaker_start(pending_freq);
        pit_sleep(pending_ms);
        speaker_stop();
        pending_freq = 0;
    }
}

