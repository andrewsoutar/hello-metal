#ifndef HELLO_METAL_PIC_H
#define HELLO_METAL_PIC_H

#include <stdint.h>

void pic_init(void);
void pic_disable(void);
void pic_eoi(uint8_t);

#endif /* HELLO_METAL_PIC_H */
