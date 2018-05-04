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

static struct rsdp const *find_rsdp_in(void const *base, void const *top) {
  for (char const *ptr = base; ptr < (char const *) top; ptr += 0x10)
    if (is_rsdp((void const *) ptr))
      return (void const *) ptr;
  return NULL;
}

static struct rsdp const *find_rsdp(void) {
  struct rsdp const *rsdp;
  char const *ebda_base = (char *) (uintptr_t) (*((uint16_t *) 0x40E) << 4);
  if ((rsdp = find_rsdp_in(ebda_base, ebda_base + 0x400)))
    return rsdp;
  if ((rsdp = find_rsdp_in((void const *) 0xE0000, (void const *) 0xFFFFF)))
    return rsdp;
  return NULL;
}

static int is_valid_table(struct description_header const *hdr) {
  return hdr && checksum(hdr, hdr->length) == 0;
}

static struct fadt const *fadt = NULL;

static struct madt const *madt = NULL;

void acpi_init(void) {
  struct rsdp const *rsdp;
  if ((rsdp = find_rsdp()) == NULL) {
    term_print("Unable to find RSDP\n");
    return;
  }

  struct description_header const *const *iter, *const *iter_end;
  size_t iter_width;
  if (rsdp->revision > 0) {
    struct xsdt const *xsdt = (void const *) (uintptr_t) rsdp->xsdt_address;
    if (!memcmp("XSDT", xsdt->hdr.signature, sizeof(xsdt->hdr.signature)) && is_valid_table(&xsdt->hdr)) {
      iter = (void const *) (uintptr_t) &xsdt->entries;
      iter_end = (void const *) ((uintptr_t) iter + xsdt->hdr.length);
      iter_width = 8;
      goto do_iter;
    }
  }

  {
    struct rsdt const *rsdt = (void const *) (uintptr_t) rsdp->rsdt_address;
    if (!memcmp("RSDT", rsdt->hdr.signature, sizeof(rsdt->hdr.signature)) && is_valid_table(&rsdt->hdr)) {
      iter = (void const *) (uintptr_t) &rsdt->entries;
      iter_end = (void const *) ((uintptr_t) iter + rsdt->hdr.length);
      iter_width = 4;
      goto do_iter;
    }
  }

  term_print("Unable to find valid XSDT or RSDT\n");
  return;

 do_iter:
  for (; iter < iter_end; iter = (void const *) ((uintptr_t) iter + iter_width)) {
    if (is_valid_table(*iter)) {
      if (!memcmp("FACP", (*iter)->signature, sizeof((*iter)->signature)))
        fadt = (struct fadt const *) *iter;
      else if (!memcmp("APIC", (*iter)->signature, sizeof((*iter)->signature)))
        madt = (struct madt const *) *iter;
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
