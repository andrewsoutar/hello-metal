#ifndef HELLO_METAL_INTERRUPT_H
#define HELLO_METAL_INTERRUPT_H

#include <stdint.h>

/* Clear interrupt flag (prevent interrupts) */
void cli(void);
/* Set interrupt flag (allow interrupts) */
void sti(void);

void interrupt_init(void);
void eoi(uint8_t);
void add_handler(uint32_t, void (*)(uint32_t, uint32_t));

#endif /* HELLO_METAL_INTERRUPT_H */
