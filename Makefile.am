noinst_PROGRAMS = hello_metal

hello_metal_SOURCES = 				\
	cpuid.c cpuid.h							\
	cpuio.c cpuio.h							\
	em64t.c em64t.h							\
	entry.S 										\
	gdt.c gdt.h									\
	hlt.c hlt.h									\
	interrupt.c interrupt.h 		\
	interrupt_stub.S						\
	kbd.c kbd.h									\
	main.c  										\
	msr.c msr.h									\
	multiboot.c multiboot.h 		\
	pic.c pic.h									\
	string.c string.h						\
	term.c term.h	term_init.S

EXTRA_hello_metal_DEPENDENCIES = multiboot.lds

# -ffreestanding tells GCC that the standard C library is not available
#
# -fno-stack-protector because otherwise GCC links against the
# __stack_chk_fail function, which is unavailable without the c runtime
# library (which we don't have)
AM_CFLAGS = -m32 -g -ffreestanding -fno-stack-protector -O3 -ffat-lto-objects
AM_CCASFLAGS = -m32 -g
# -n to turn off alignment, which can move the multiboot header out of
# range
#
# --build-id=none because it takes up space and displaces the multiboot
# --header
hello_metal_LDFLAGS = -T $(top_srcdir)/multiboot.lds -nostdlib -Wl,-n,--build-id=none

.PHONY: run-qemu run-qemu-gdb
run-qemu : hello_metal
	qemu-system-i386 $(QEMUFLAGS) -kernel $<
run-qemu-gdb : hello_metal
	qemu-system-i386 $(QEMUFLAGS) -kernel $< -s -S
grub.img : hello_metal
	cp $< grub/hello_metal
	grub-mkrescue --compress=xz -o $@ grub
