#include "mem.h"

#include <stdint.h>

#include "rbtree.h"
#include "string.h"
#include "utils.h"

#define PT_PRESENT (1u << 0)
#define PT_WRITE (1u << 1)
#define PT_USER (1u << 2)
#define PT_SIZE (1u << 7)

typedef uint64_t page_table[512] __attribute__((aligned(4096)));

static page_table early_pd = {
  0x00000000 | PT_PRESENT | PT_WRITE | PT_SIZE /* Identity map first 2M using hugepage */
};
static const page_table early_pdpt = {
  (uint64_t) &early_pd | PT_PRESENT | PT_WRITE /* Early page directory uses first 1GB */
};
page_table pml4 = {
  (uint64_t) &early_pdpt | PT_PRESENT | PT_WRITE, /* Early PDPT uses first 512GB */
  [511] = (uint64_t) &pml4 | PT_PRESENT | PT_WRITE /* Recursive mapping to modify page tables */
};

static void *invlpg(uint64_t virt_addr) {
  asm volatile("invlpg (%[virt_addr])" :: [virt_addr] "r"(virt_addr) : "memory");
}

#define PT_SHIFT 12
#define PD_SHIFT (9 + PT_SHIFT)
#define PDPT_SHIFT (9 + PD_SHIFT)
#define PML4_SHIFT (9 + PDPT_SHIFT)
#define VM_WIDTH (9 + PML4_SHIFT)
uint64_t *pte(uint64_t vaddr, uint64_t level_shift) {
  return sizeof uint64_t *
    (((uint64_t) -1 << VM_WIDTH - level_shift) +
     (vaddr % (1 << VM_WIDTH) >> level_shift));
}

void *map_region_early(uint64_t phys_addr, size_t size) {
  if (size == 0)
    return NULL;

  /* I don't want to assume 1G-page support, and since I only ever map
   * one thing at a time there's no reason to use smaller than
   * 2M-pages */
  uint64_t map_base = phys_addr >> PD_SHIFT,
    /* size > 0, so phys_addr + size - 1 will never underflow */
    map_limit = ((phys_addr + size - 1) >> PD_SHIFT) + 1;

  if (map_base == 0)
    /* The lowest page is already identity-mapped, and our mappings
     * are contiguous with the idmap */
    ++map_base;

  if (map_limit - map_base > 511)
    /* We have 512 entries, and one is take by the idmap */
    return NULL;

  for (size_t i = 0; map_base + i < map_limit; ++i)
    /* Skip the first hugepage, which is the idmap */
    *pte(1 + i << PD_SHIFT, PD_SHIFT) =
      ((map_base + i) << PD_SHIFT) | PT_PRESENT | PT_WRITE | PT_SIZE;
  for (size_t i = 0; map_base + i < map_limit; ++i)
    /*
     * At this point only hugepages could be valid page table entries
     * since we set up the page table; therefore it's fine to invlpg
     * the whole hugepage with one call.  invlpg is a serializing
     * instruction, by batching it at least we keep pipeline bubbles
     * all in one place
     */
    invlpg(1 + i << PD_SHIFT);

  if (phys_addr < ((uint64_t) 1 << PD_SHIFT))
    /* We started in the idmap, therefore the whole thing is idmapped */
    return (void *) phys_addr;
  else
    return (void *) (((uint64_t) 1 << PD_SHIFT) + (phys_addr % ((uint64_t) 1 << PD_SHIFT)));
}


static struct virt_map {
  struct rbtree_node tree_node;
  uint64_t base;
  bool used;
} *virt_map_root = NULL, *virt_map_free_list;

static struct virt_map *virt_map_node_alloc() {
  struct virt_map *ret = virt_map_free_list;
  virt_map_free_list = ret->tree_node.left;
  if (virt_map_free_list == NULL) {
    virt_map_free_list = ret + 1;
    if ((uint64_t) ret % ((uint64_t) 1 << 12) !=
        (uint64_t) virt_map_free_list % ((uint64_t) 1 << 12)) {
      /* Ran off the end of the page, bail out */
      virt_map_free_list = ret;
      return NULL;
    }
  }
  return ret;
}
static void virt_map_node_dealloc(struct virt_map *node) {
  node->tree_node.left = virt_map_free_list;
  virt_map_free_list = node;
}

static void virt_alloc_init(const uint64_t init_frames[static 4]) {
  /* Take one page to be our initial PDPT */

  /* Physical frames: */
  uint64_t pdpt_pf = init_pages[0];
  uint64_t pd_pf = init_pages[1];
  uint64_t pt_pf = init_pages[2];
  uint64_t vmm_boot = init_pages[3];

  uint64_t available_vm_start = (uint64_t) 1 << PD_SHIFT;

  page_table *pdpt = map_region_early(pdpt_pf);
  memset(pdpt, 0, sizeof *pdpt);
  (*pdpt)[0] = pd_pf | PT_PRESENT | PT_WRITE;

  page_table *pd = map_region_early(pd_pf);
  memset(pd, 0, sizeof *pd);
  /* Identity map */
  (*pd)[0] = 0x00000000 | PT_PRESENT | PT_WRITE | PT_SIZE;

  /* Now we have enough to pivot to the new mappings */
  /* We can use the recursive mappings to modify the PML4 */
  *pte(0, PML4_SHIFT) = pdpt_pf | PT_PRESENT | PT_WRITE;
  invlpg((uint64_t) pte(0, PDPT_SHIFT));

  /* Set up first page table */
  *pte(available_vm_start, PD_SHIFT) = pt_pf | PT_PRESENT | PT_WRITE;
  invlpg((uint64_t) pte(available_vm_start, PT_SHIFT));

  /* Map vmm_boot at 2M */
  *pte(available_vm_start, PT_SHIFT) = vmm_boot | PT_PRESENT | PT_WRITE;
  invlpg(available_vm_start);
  virt_map_free_list = (void *) available_vm_start;
  virt_map_free_list->tree_node.left = NULL;

  /* Make sure we have enough room to allocate two nodes */
  static_assert(3 * sizeof (struct virt_map) <= (uint64_t) 1 << PT_SHIFT);

  virt_map_root = virt_map_node_alloc();
  *virt_map_root = (struct virt_map) {
    .tree_node.black = true,
    .base = (uint64_t) 1 << 21 + (uint64_t) 1 << PT_SHIFT,
    .used = false
  };
  virt_map_root->tree_node.left = virt_map_node_alloc();
  *virt_map_root->tree_node.left = (struct virt_map) {
    .tree_node.black = false,
    .base = 0,
    .used = true
  };
}

void phys_alloc_add_frame(uint64_t phys_frame) {
  static uint64_t init_frames[4];
  static size_t init_frames_i = 0;

  if (init_frames_i < ARRAY_LEN(init_frames)) {
    init_frames[init_frames_i++] = 
  }
}
