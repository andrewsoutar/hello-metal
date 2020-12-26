noinst_PROGRAMS = hello_metal

hello_metal_SOURCES =     \
	acpi.c acpi.h acpi_struct.h \
	apic.c apic.h           \
  cpuid.c cpuid.h         \
	cpuio.c cpuio.h         \
	entry.S                 \
  gdt.c gdt.h             \
	hlt.c hlt.h             \
	main.c                  \
	mem.c mem.h             \
	msr.c msr.h             \
	multiboot.S	multiboot.h \
	pic.c pic.h             \
	string.c string.h       \
	term.c term.h

# -ffreestanding tells GCC that the standard C library is not available
#
# -fno-stack-protector because otherwise GCC links against the
# __stack_chk_fail function, which is unavailable without the c runtime
# library (which we don't have)
hello_metal_CFLAGS = -g -ffreestanding -fno-stack-protector -O3 -ffat-lto-objects -flto
hello_metal_CCASFLAGS = -g

EXTRA_hello_metal_DEPENDENCIES = multiboot.lds

# -n to turn off alignment, which can move the multiboot header out of
# range
#
# --build-id=none because it takes up space and displaces the multiboot
# --header
hello_metal_LDFLAGS = -no-pie -T $(top_srcdir)/multiboot.lds -nostdlib -Wl,-n,--build-id=none

if HAVE_GRUB_MKRESCUE

.PHONY: run-qemu run-qemu-gdb

grub/hello_metal: hello_metal
	cp "$<" "$@"
grub.img : grub/hello_metal $(wildcard grub/**/*)
	$(GRUB_MKRESCUE) --compress=xz -o $@ grub
run-qemu : grub.img
	qemu-system-x86_64 $(QEMUFLAGS) --drive media=cdrom,file=$<
run-qemu-gdb : grub.img
	qemu-system-x86_64 $(QEMUFLAGS) --drive media=cdrom,file=$< -S -s

endif
