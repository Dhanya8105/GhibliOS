#include "ac97.h"
#include "pci.h"
#include "../kernel/pit.h"
#include <stdint.h>

#define AC97_VENDOR_ID 0x8086
#define AC97_DEVICE_ID 0x2415

/* NAM (mixer) register offsets */
#define NAM_MASTER_VOL   0x02
#define NAM_PCM_VOL      0x18
#define NAM_EXT_AUDIO_SC 0x2A  /* Extended Audio Status/Control */
#define NAM_FRONT_RATE   0x2C  /* PCM Front DAC Rate */

#define EASC_VRA         0x0001 /* Variable Rate Audio enable */

/* NABM (bus master) register offsets, PCM OUT channel */
#define PO_BDBAR   0x10
#define PO_LVI     0x15
#define PO_CR      0x1B

#define CR_RPBM    0x01
#define CR_LVBIE   0x04
#define CR_IOCE    0x10

typedef struct {
    uint32_t addr;
    uint16_t len;
    uint16_t flags;
} __attribute__((packed)) bdl_entry_t;

#define BDL_IOC 0x8000

#define BDL_ENTRIES 32
static bdl_entry_t bdl[BDL_ENTRIES] __attribute__((aligned(8)));

static uint16_t nabm_base = 0;
static uint16_t nam_base  = 0;

int ac97_init(void) {
    uint8_t bus, slot, func;
    if (!pci_find_device(AC97_VENDOR_ID, AC97_DEVICE_ID, &bus, &slot, &func))
        return 0;

    uint32_t bar0 = pci_config_read32(bus, slot, func, 0x10);
    uint32_t bar1 = pci_config_read32(bus, slot, func, 0x14);

    nam_base  = (uint16_t)(bar0 & 0xFFFC);
    nabm_base = (uint16_t)(bar1 & 0xFFFC);

    uint32_t addr = (uint32_t)((1u << 31) | (bus << 16) | (slot << 11) |
                               (func << 8) | 0x04);
    outl(0xCF8, addr);
    uint32_t cmd = inl(0xCFC);
    cmd |= 0x0005;
    outl(0xCF8, addr);
    outl(0xCFC, cmd);

    /* Unmute, max volume (0 = loudest on AC97's inverted scale) */
    outw(nam_base + NAM_MASTER_VOL, 0x0000);
    outw(nam_base + NAM_PCM_VOL, 0x0000);

    /* Enable variable rate audio, then set DAC rate to 44100 Hz
       to match our embedded PCM data exactly. */
    outw(nam_base + NAM_EXT_AUDIO_SC, EASC_VRA);
    outw(nam_base + NAM_FRONT_RATE, 22050);
  	

    return 1;
}

void ac97_play_loop(const unsigned char *data, unsigned int byte_len) {
    /* AC97 buffer length field counts 16-bit samples, not bytes */
    uint32_t total_samples = byte_len / 2;
    uint32_t addr = (uint32_t)data;
    int i = 0;

    while (total_samples > 0 && i < BDL_ENTRIES) {
        uint32_t chunk = total_samples;
        if (chunk > 0xFFFE) chunk = 0xFFFE;

        bdl[i].addr  = addr;
        bdl[i].len   = (uint16_t)chunk;
        bdl[i].flags = BDL_IOC;

        addr += chunk * 2; /* advance by bytes, 2 bytes per sample */
        total_samples -= chunk;
        i++;
    }
    int last = i - 1;
    if (last < 0) return;

    outl(nabm_base + PO_BDBAR, (uint32_t)bdl);
    outb(nabm_base + PO_LVI, (uint8_t)last);
    outb(nabm_base + PO_CR, CR_RPBM);
  
}
