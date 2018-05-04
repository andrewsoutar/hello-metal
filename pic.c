#include "pic.h"

#include <stddef.h>

#include "cpuio.h"

#define ICW1      (1u<<4)
#define ICW1_LTIM (1u<<3)
#define ICW1_ADI  (1u<<2)
#define ICW1_SNGL (1u<<1)
#define ICW1_ICW4 (1u<<0)

#define ICW4_SFNM (1u<<4)
#define ICW4_BUF  (1u<<3)
#define ICW4_MS   (1u<<2)
#define ICW4_AEOI (1u<<1)
#define ICW4_uPM  (1u<<0)

#define OCW2      (0)
#define OCW2_R    (1u<<7)
#define OCW2_SL   (1u<<6)
#define OCW2_EOI  (1u<<5)

#define OCW3      (0b01u<<3)
#define OCW3_ESMM (1u<<6)
#define OCW3_SMM  (1u<<5)
#define OCW3_P    (1u<<2)
#define OCW3_RR   (1u<<1)
#define OCW3_RIS  (1u<<0)

#define OCW3_ISR  (OCW3_RR | OCW3_RIS)

#define EOI       (0x20)

static const struct pic {
  uint16_t io_addr;
  uint8_t offset;
  uint8_t slave_mask;
} pics[] = {
  {0x0020u, 0x20u, 1u << 0x02u},
  {0x00A0u, 0x28u, 0x02u}
};

static void pic_out(struct pic const *pic, uint8_t a0, uint8_t data) {
  outb(pic->io_addr | a0, data);
  io_wait();
}

static void pic_icw(struct pic const *pic) {
  pic_out(pic, 0, ICW1 | ICW1_ICW4);
  pic_out(pic, 1, pic->offset);
  pic_out(pic, 1, pic->slave_mask);
  pic_out(pic, 1, ICW4_uPM);
}

void pic_init(void) {
  for (size_t i = 0; i < sizeof(pics) / sizeof(*pics); ++i) {
    pic_icw(&pics[i]);
    pic_out(&pics[i], 1, 0x00);
  }
}

void pic_disable(void) {
  for (size_t i = 0; i < sizeof(pics) / sizeof(*pics); ++i) {
    pic_icw(&pics[i]);
    pic_out(&pics[i], 1, 0xFF);
  }
}

void pic_eoi(uint8_t irq) {
  if (irq >= 8)
    pic_out(&pics[1], 0, EOI);
  pic_out(&pics[0], 0, EOI);
}
