#include "em64t.h"

#include "cpuid.h"
#include "gdt.h"
#include "msr.h"

/* For debugging */
#include "term.h"

int em64t_mode() {
  struct cpuid cpuid_;
  return (cpuid(&cpuid_, 0x80000001, 0) == 0) && (cpuid_.edx & CPUID_EM64T);
}

/* GDT with 64-bit (long mode) entries */
static const struct gdt_entry long_gdt_entries[] = {
  {0},                          /* First segment is unused */
  {                             /* 64-bit code segment */
   .limit_low = 0xFFFFu,
   .base_low  = 0x0000u,
   .base_mid  = 0x00u,
   .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
   .limit_high_flags = 0xFu | GDT_L | GDT_G,
   .base_high = 0x00u
  },
  {                             /* 32/64-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  }
};
static const struct gdt long_gdt = {
  sizeof(long_gdt_entries)-1,
  (uint32_t) (void const *) long_gdt_entries
};

typedef uint64_t pt[512] __attribute__((aligned(4096)));
static pt pml4 = {0}, idmap1 = {0}, idmap2 = {0};
#define PT_PRESENT (1u << 0)
#define PT_WRITE (1u << 1)
#define PT_USER (1u << 2)
#define PT_SIZE (1u << 7)

void em64t_enter() {
  uint32_t tmp;

  term_print("Enabling PAE...");
  asm("movl %%cr4, %[cr4]" : [cr4] "=r" (tmp));
  asm volatile("movl %[cr4], %%cr4" :: [cr4] "r" (tmp | (1u << 5)));
  term_print(" done\n");

  term_print("Enabling EM64T mode...");
  wrmsr(0xC0000080, rdmsr(0xC0000080) | (1u << 8));
  term_print(" done\n");

  /* Load PML4 address into CR3 */
  pml4[0] = ((uint64_t) (uintptr_t) &idmap1) | PT_PRESENT | PT_WRITE | PT_USER;
  pml4[511] = ((uint64_t) (uintptr_t) &pml4) | PT_PRESENT | PT_WRITE;
  idmap1[0] = ((uint64_t) (uintptr_t) &idmap2) | PT_PRESENT | PT_WRITE | PT_USER;
  idmap2[0] = PT_PRESENT | PT_WRITE | PT_USER | PT_SIZE;
  term_print("Loading page table...");
  asm volatile("movl %[pml4], %%cr3" :: [pml4] "r" (pml4));
  term_print(" done\n");
  
  /* Enable paging : CR0.PG = 1 */
  term_print("Enabling paging...");
  asm("movl %%cr0, %[cr0]" : [cr0] "=r" (tmp));
  asm volatile("movl %[cr0], %%cr0" :: [cr0] "r" (tmp | (1u << 31)));
  term_print(" done\n");
}
