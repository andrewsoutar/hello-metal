OUTPUT_FORMAT ("elf64-x86-64");

ENTRY (_start);

SECTIONS {
        _low_addr = 0x7000;

        . = 1M;

        .text ALIGN (4K) : AT(ADDR(.text)) ALIGN (4K) {
                . = ALIGN(8);
                KEEP(*(.multiboot))
                *(.text)
                *(.text.*)
        }
        .rodata ALIGN (4K) : AT(ADDR(.rodata)) ALIGN (4K) {
                *(.rodata)
                *(.rodata.*)
        }
        .data ALIGN (4K) : AT(ADDR(.data)) ALIGN (4K) {
                *(.data)
                *(.data.*)
        }
        .bss ALIGN (4K) : AT(ADDR(.bss)) ALIGN (4K) {
                *(.dynbss)
                *(.bss)
                *(.bss.*)
                *(COMMON)
        }

        .stack ALIGN (4K) : AT(ADDR(.stack)) ALIGN (4K) {
                _stack_start = .;
                . += 32K;
                _stack_end = .;
        }
}
