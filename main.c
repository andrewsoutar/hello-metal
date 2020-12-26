#include "hlt.h"
#include "msr.h"
#include "multiboot.h"
#include "term.h"
#include "vmx.h"

void main(uint32_t multiboot_magic, struct multiboot_data *multiboot_data) {
  if (multiboot_magic != MBOOT_REG_MAGIC)
    return;

  term_clr();

  /* Hello, metal! */
  term_print("Hello, bare metal!\n");

  if (has_vmx()) {
    term_print("We have VMX\n");
    vmxon();

    uint64_t vmx_basic_msr = rdmsr(0x00000480u);
    uint32_t vmcs_revision = vmx_basic_msr & 0x7FFFFFFFu;
    uint32_t vmcs_size = (vmx_basic_msr >> 32) & 0x00001FFF;

    unsigned char vmcs[vmcs_size];

    memset(vmcs, 0, sizeof(vmcs));
    memcpy(&vmcs[0], &vmcs_revision, 4);
    
  } else {
    term_print("No VMX :(\n");
  }

  for (;;) hlt();
}
