#ifndef HELLO_METAL_ACPI_H
#define HELLO_METAL_ACPI_H

#include "acpi_struct.h"

void acpi_init(void const *rsdp);

int acpi_has_8042(void);
struct madt const *acpi_get_madt(void);

#endif /* HELLO_METAL_ACPI_H */
