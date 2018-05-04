#ifndef HELLO_METAL_ACPI_H
#define HELLO_METAL_ACPI_H

void acpi_init(void);
int acpi_has_8042(void);

struct madt const *acpi_get_madt(void);

#endif /* HELLO_METAL_ACPI_H */
