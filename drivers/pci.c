#include "../kernel/pit.h"
#include "pci.h"

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

static uint32_t pci_make_addr(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    return (uint32_t)((1u << 31) | (bus << 16) | (slot << 11) |
                      (func << 8) | (offset & 0xFC));
}

uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    outl(PCI_CONFIG_ADDR, pci_make_addr(bus, slot, func, offset));
    return inl(PCI_CONFIG_DATA);
}

uint16_t pci_config_read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t v = pci_config_read32(bus, slot, func, offset);
    return (uint16_t)((v >> ((offset & 2) * 8)) & 0xFFFF);
}

int pci_find_device(uint16_t vendor_id, uint16_t device_id,
                    uint8_t *out_bus, uint8_t *out_slot, uint8_t *out_func) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint16_t vid = pci_config_read16((uint8_t)bus, slot, func, 0x00);
                if (vid == 0xFFFF) continue;
                uint16_t did = pci_config_read16((uint8_t)bus, slot, func, 0x02);
                if (vid == vendor_id && did == device_id) {
                    *out_bus = (uint8_t)bus;
                    *out_slot = slot;
                    *out_func = func;
                    return 1;
                }
            }
        }
    }
    return 0;
}
