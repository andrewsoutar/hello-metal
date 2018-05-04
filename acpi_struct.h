#ifndef HELLO_METAL_ACPI_STRUCT_H
#define HELLO_METAL_ACPI_STRUCT_H

#include <stdint.h>

struct rsdp {
  /* v1 */
  uint8_t signature[8];
  uint8_t checksum;
  uint8_t oemid[6];
  uint8_t revision;
  uint32_t rsdt_address;

  /* v2 */
  uint32_t length;
  uint64_t xsdt_address;
  uint8_t extended_checksum;
} __attribute__((packed));

struct description_header {
  uint8_t signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  uint8_t oemid[6];
  uint8_t oem_table_id[8];
  uint32_t oem_revision;
  uint8_t creator_id[4];
  uint32_t creator_revision;
} __attribute__((packed));

struct rsdt {
  struct description_header hdr;
  uint32_t entries[];
} __attribute__((packed));

struct xsdt {
  struct description_header hdr;
  uint64_t entries[];
};

struct fadt {
  struct description_header hdr;
  uint32_t firmware_ctrl;
  uint32_t dsdt;
  uint8_t int_model;
  uint8_t preferred_pm_profile;
  uint16_t sci_int;
  uint32_t smi_cmd;
  uint8_t acpi_enable;
  uint8_t acpi_disable;
  uint8_t s4bios_req;
  uint8_t pstate_cnt;
  uint32_t pm1a_evt_blk;
  uint32_t pm1b_evt_blk;
  uint32_t pm1a_cnt_blk;
  uint32_t pm1b_cnt_blk;
  uint32_t pm2_cnt_blk;
  uint32_t pm_tmr_blk;
  uint32_t gpe0_blk;
  uint32_t gpe1_blk;
  uint8_t pm1_evt_len;
  uint8_t pm1_cnt_len;
  uint8_t pm2_cnt_len;
  uint8_t pm_tmr_len;
  uint8_t gpe0_blk_len;
  uint8_t gpe1_blk_len;
  uint8_t gpe1_base;
  uint8_t cst_cnt;
  uint16_t p_lvl2_lat;
  uint16_t p_lvl3_lat;
  uint16_t flush_size;
  uint16_t flush_stride;
  uint8_t duty_offset;
  uint8_t duty_width;
  uint8_t day_alrm;
  uint8_t mon_alrm;
  uint8_t century;
  uint16_t iapc_boot_arch;
#define FADT_IAPC_LEGACY_DEVICES (1u<<0)
#define FADT_IAPC_8042 (1u<<1)
#define FADT_IAPC_VGA_NP (1u<<2)
#define FADT_IAPC_MSI_NS (1u<<3)
#define FADT_IAPC_PCIE_ASPM (1u<<4)
#define FADT_IAPC_CMOS_RTC_NP (1u<<5)
  uint8_t rsvd1;
  uint32_t flags;
  uint32_t reset_reg[3];
  uint8_t reset_value;
  uint16_t arm_boot_arch;
  uint8_t minor_version;
  uint64_t x_firmware_ctrl;
  uint64_t x_dsdt;
  uint32_t x_pm1a_evt_blk[3];
  uint32_t x_pm1b_evt_blk[3];
  uint32_t x_pb1a_cnt_blk[3];
  uint32_t x_pb1b_cnt_blk[3];
  uint32_t x_pm2_cnt_blk[3];
  uint32_t x_pm_tmr_blk[3];
  uint32_t x_gpe0_blk[3];
  uint32_t x_gpe1_blk[3];
  uint32_t sleep_control_reg[3];
  uint32_t sleep_status_reg[3];
  uint64_t hypervisor_vendor_id;
} __attribute__((packed));

struct madt {
  struct description_header hdr;
  uint32_t local_addr;
  uint32_t flags;
#define MADT_PCAT_COMPAT (1u<<0)
  struct ics {
    uint8_t type;
    uint8_t length;
    union {
#define MADT_PROC_APIC 0u
      struct proc_apic {
        uint8_t proc_uid;
        uint8_t apic_id;
        uint32_t flags;
#define MADT_PROC_APIC_USABLE (1u<<0)
      } __attribute__((packed)) proc_apic;

#define MADT_IOAPIC 1u
      struct ioapic {
        uint8_t ioapic_id;
        uint8_t rsvd1;
        uint32_t addr;
        uint32_t int_base;
      } __attribute__((packed)) ioapic;

#define MADT_INT_SRC_OVERRIDE 2u
      struct int_src_override {
        uint8_t rsvd1;
        uint8_t src_irq;
        uint32_t gsi;
        uint16_t flags;
      } __attribute__((packed)) int_src_override;

#define MADT_NMI_SRC 3u
      struct nmi_src {
        uint16_t flags;
        uint32_t gsi;
      } __attribute__((packed)) nmi_src;
    };
  } __attribute__((packed)) ics[];
} __attribute__((packed));

#endif /* HELLO_METAL_ACPI_STRUCT_H */
