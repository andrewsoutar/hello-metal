#include "apic.h"

#include <stddef.h>

#include "acpi.h"
#include "acpi_struct.h"
#include "cpuid.h"
#include "pic.h"

int apic_present(void) {
  struct cpuid id;
  return cpuid(&id, 1, 0) != 0 && (id.edx & CPUID_APIC) != 0 && acpi_get_madt();
}

static void volatile *lapic_addr(void) {
  return (void volatile *) (uintptr_t) acpi_get_madt()->local_addr;
}

#define LAPIC_REG_ID (0x0020)
#define LAPIC_REG_SIV (0x00F0)
#define LAPIC_REG_ICR (0x0300)
#define LAPIC_REG_ICR_HIGH (0x0310)

#define LAPIC_SW_EN (1 << 8)

static uint32_t lapic_read(uint16_t reg) {
  return *(uint32_t volatile *) ((char volatile *) lapic_addr() + reg);
}

static void lapic_write(uint16_t reg, uint32_t value) {
  *(uint32_t volatile *) ((char volatile *) lapic_addr() + reg) = value;
}

static void ioregsel(void volatile *ioapic_base, uint8_t reg) {
  *(uint32_t volatile *) ioapic_base = reg;
}

#define IOAPIC_VER 0x01u
#define IOAPIC_REDTBL(n) (0x10u + 2*(n))

static uint32_t ioapic_read(void volatile *ioapic_base, uint8_t reg) {
  ioregsel(ioapic_base, reg);
  return *(uint32_t volatile *) ((uintptr_t) ioapic_base | 0x10);
}

static void ioapic_write(void volatile *ioapic_base, uint8_t reg, uint32_t value) {
  ioregsel(ioapic_base, reg);
  *(uint32_t volatile *) ((uintptr_t) ioapic_base | 0x10) = value;
}

void apic_init(void) {
  struct madt const *madt = acpi_get_madt();
  if (acpi_has_8042())
    pic_disable();
  lapic_write(LAPIC_REG_SIV, LAPIC_SW_EN | 0xFF);

  uint8_t bsp_id = lapic_read(LAPIC_REG_ID) >> 24;

  struct int_src_override const *int1_override = NULL;
  struct int_src_override const *int12_override = NULL;
  for (struct ics const *ics = madt->ics; (char const *) ics < (char const *) madt + madt->hdr.length;
       ics = (struct ics const *) ((char const *) ics + ics->length)) {
    if (ics->type == MADT_INT_SRC_OVERRIDE) {
      if (ics->int_src_override.src_irq == 1)
        int1_override = &ics->int_src_override;
      else if (ics->int_src_override.src_irq == 12)
        int12_override = &ics->int_src_override;
    }
  }

  uint32_t int1_gsi = int1_override ? int1_override->gsi : 1;
  uint32_t int12_gsi = int12_override ? int12_override->gsi : 12;

  for (struct ics const *ics = madt->ics; (char const *) ics < (char const *) madt + madt->hdr.length;
       ics = (struct ics const *) ((char const *) ics + ics->length)) {
    if (ics->type == MADT_IOAPIC) {
      void volatile *ioapic_base = (void volatile *) (uintptr_t) ics->ioapic.addr;
      uint32_t low_gsi = ics->ioapic.int_base;
      uint32_t top_gsi = low_gsi + ((ioapic_read(ioapic_base, IOAPIC_VER) >> 16) & 0xFF);

      if (low_gsi <= int1_gsi && top_gsi >= int1_gsi) {
        uint32_t tmp = ioapic_read(ioapic_base, IOAPIC_REDTBL(int1_gsi - low_gsi)+1);
        ioapic_write(ioapic_base, IOAPIC_REDTBL(int1_gsi - low_gsi)+1, (tmp & 0x00FFFFFFu) | (bsp_id << 24));
        ioapic_write(ioapic_base, IOAPIC_REDTBL(int1_gsi - low_gsi),
                     0x21 | ((int1_override && (int1_override->flags & 0x03) == 0x03) << 13) |
                     ((int1_override && (int1_override->flags & 0xC) == 0xC) << 15));
      }
      if (low_gsi <= int12_gsi && top_gsi >= int12_gsi) {
        uint32_t tmp = ioapic_read(ioapic_base, IOAPIC_REDTBL(int12_gsi - low_gsi)+1);
        ioapic_write(ioapic_base, IOAPIC_REDTBL(int12_gsi - low_gsi)+1, (tmp & 0x00FFFFFFu) | (bsp_id << 24));
        ioapic_write(ioapic_base, IOAPIC_REDTBL(int12_gsi - low_gsi),
                     0x2C | ((int12_override && (int12_override->flags & 0x03) == 0x03) << 13) |
                     ((int12_override && (int12_override->flags & 0xC) == 0xC) << 15));
      }
    }
  }
}

void apic_eoi(void) {
  lapic_write(0xB0, 0);
}
