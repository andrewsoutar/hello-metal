#include "acpi.h"
#include "acpi_struct.h"

#include "string.h"
#include "term.h"

static uint8_t checksum(void const *data, size_t length) {
  uint8_t const *data_ = data;
  uint8_t checksum = 0;
  while (length--) checksum += data_[length];
  return checksum;
}

static int is_rsdp(struct rsdp const *rsdp) {
  if (memcmp("RSD PTR ", rsdp->signature, sizeof(rsdp->signature)) != 0)
    return 0;

  /*
   * (struct rsdp)->length is the first v2 field - everything before it
   * is v1
   */
  if (checksum(rsdp, offsetof(struct rsdp, length)) != 0) {
    term_print("Checksum failed\n");
    return 0;
  }

  if (rsdp->revision == 0)
    return 1;

  if (rsdp->length < sizeof(rsdp)) {
    term_print("Length check failed\n");
    return 0;
  }
  if (checksum(rsdp, rsdp->length) != 0) {
    term_print("Long checksum failed\n");
    return 0;
  }

  return 2;
}

static int is_valid_table(struct description_header const *hdr) {
  return hdr && checksum(hdr, hdr->length) == 0;
}

static struct fadt const *fadt = NULL;

static struct madt const *madt = NULL;

void acpi_init(void const *rsdp_) {
  struct rsdp const *rsdp = rsdp_;
  if (!is_rsdp(rsdp)) {
    term_print("Bad RSDP provided! Bailing out\n");
    return;
  }

  char const *iter, *iter_end;
  size_t iter_width;
  if (rsdp->revision > 0) {
    struct xsdt const *xsdt = (void const *) (uintptr_t) rsdp->xsdt_address;
    if (!memcmp("XSDT", xsdt->hdr.signature, sizeof(xsdt->hdr.signature)) && is_valid_table(&xsdt->hdr)) {
      iter = (char const *) &xsdt->entries;
      iter_end = iter + xsdt->hdr.length;
      iter_width = 8;
      goto do_iter;
    }
  }

  {
    struct rsdt const *rsdt = (void const *) (uintptr_t) rsdp->rsdt_address;
    if (!memcmp("RSDT", rsdt->hdr.signature, sizeof(rsdt->hdr.signature)) && is_valid_table(&rsdt->hdr)) {
      iter = (char const *) &rsdt->entries;
      iter_end = iter + rsdt->hdr.length;
      iter_width = 4;
      goto do_iter;
    }
  }

  term_print("Unable to find valid XSDT or RSDT\n");
  return;

 do_iter:
  for (; iter < iter_end; iter = (void const *) ((uintptr_t) iter + iter_width)) {
    struct description_header const *header;
    if (iter_width == 8) {
      uint64_t tmp;
      memcpy(&tmp, iter, iter_width);
      header = (void const *) (uintptr_t) tmp;
    } else if (iter_width == 4) {
      uint32_t tmp;
      memcpy(&tmp, iter, iter_width);
      header = (void const *) (uintptr_t) tmp;
    }
    if (is_valid_table(header)) {
      if (!memcmp("FACP", header->signature, sizeof(header->signature)))
        fadt = (struct fadt const *) header;
      else if (!memcmp("APIC", header->signature, sizeof(header->signature)))
        madt = (struct madt const *) header;
      /* TODO detect other important structures here */
    }
  }
  if (!fadt) {
    term_print("Unable to find valid FADT\n");
    return;
  }
}

int acpi_has_8042(void) {
  if (!fadt)
    /* Probably the case */
    return 1;
  return fadt->iapc_boot_arch & FADT_IAPC_8042;
}

struct madt const *acpi_get_madt(void) {
  return madt;
}
