#include "mem.h"

#include <stdint.h>

#define PT_PRESENT (1u << 0)
#define PT_WRITE (1u << 1)
#define PT_USER (1u << 2)
#define PT_SIZE (1u << 7)

typedef uint64_t page_table[512] __attribute__((aligned(4096)));

static page_table early_pd = {
  0x00000000 | PT_PRESENT | PT_WRITE | PT_USER | PT_SIZE /* Identity map first 2M using largepage */
};
static const page_table early_pdpt = {
  (uint64_t) &early_pd + PT_PRESENT + PT_WRITE + PT_USER /* Early page directory uses first 1GB */
};
const page_table early_pml4 = {
  (uint64_t) &early_pdpt + PT_PRESENT + PT_WRITE + PT_USER, /* Early PDPT uses first 512GB */
  [511] = (uint64_t) &early_pml4 + PT_PRESENT + PT_WRITE /* Recursive mapping to modify page tables */
};

static void invlpg(uint64_t virt_addr) {
  asm volatile("invlpg (%[virt_addr])" :: [virt_addr] "r"(virt_addr) : "memory");
}

void *map_region_early(uint64_t phys_addr, size_t size) {
  if (size == 0)
    return NULL;

  /* I don't want to assume huge(1G)-page support, and since I only
   * ever map one thing at a time there's no reason to use smaller
   * than large(2M)-pages */
  uint64_t map_base = phys_addr >> 21,
    /* size > 0, so phys_addr + size - 1 will never underflow */
    map_limit = ((phys_addr + size - 1) >> 21) + 1;

  if (map_base == 0)
    /* The lowest page is already identity-mapped, and our mappings
     * are contiguous with the idmap */
    ++map_base;

  if (map_limit - map_base > 511)
    /* We have 512 entries, and one is take by the idmap */
    return NULL;

  for (size_t i = 0; map_base + i < map_limit; ++i)
    /* early_pd[0] is the idmap, skip that */
    early_pd[1 + i] = ((map_base + i) << 21) | PT_PRESENT | PT_WRITE | PT_USER | PT_SIZE;
  for (size_t i = 0; map_base + i < map_limit; ++i)
    invlpg((map_base + i) << 21);

  if (phys_addr < ((uint64_t) 1 << 21))
    /* We started in the idmap, therefore the whole thing is idmapped */
    return (void *) phys_addr;
  else
    return (void *) (((uint64_t) 1 << 21) | (phys_addr & ~((uint64_t) 1 << 21 - 1)));
}
