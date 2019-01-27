#include <stdint.h>

#define PT_PRESENT (1u << 0)
#define PT_WRITE (1u << 1)
#define PT_USER (1u << 2)
#define PT_SIZE (1u << 7)

typedef uint64_t page_table[512] __attribute__((aligned(4096)));

static const page_table idmap2 = { PT_PRESENT | PT_WRITE | PT_USER | PT_SIZE };
static const page_table idmap1 = { (uint64_t) idmap2 + PT_PRESENT + PT_WRITE + PT_USER };
const page_table pml4 = {
  (uint64_t) idmap1 + PT_PRESENT + PT_WRITE + PT_USER,
  [511] = (uint64_t) pml4 + PT_PRESENT + PT_WRITE
};
