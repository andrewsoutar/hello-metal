#ifndef HELLO_METAL_APIC_H
#define HELLO_METAL_APIC_H

int apic_present(void);
void apic_init(void);
void apic_eoi(void);

#endif /* HELLO_METAL_APIC_H */
