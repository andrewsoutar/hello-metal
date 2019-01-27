OUTPUT_FORMAT ("elf64-x86-64");

ENTRY (_start);

SECTIONS {
        _low_addr = 0x7000;

        . = 1M;

        .multiboot : {
                *(.multiboot)
        }

        .text ALIGN (4K) : ALIGN (4K) {
                *(.text)
                *(.text.*)
        }
        .rodata ALIGN (4K) : ALIGN (4K) {
                *(.rodata)
                *(.rodata.*)
        }
        .data ALIGN (4K) : ALIGN (4K) {
                *(.data)
                *(.data.*)
        }
        .bss ALIGN (4K) : ALIGN (4K) {
                *(.dynbss)
                *(.bss)
                *(.bss.*)
                *(COMMON)
        }

        . = ALIGN(4K);
        _low_start = .;

        . = _low_addr;
        low_disp = _low_start - _low_addr;

        .low.text ALIGN(4K) : AT(ADDR(.low.text) + low_disp) ALIGN(4K) {
                *(.low.text)
                *(.low.text.*)
        }
        .low.rodata ALIGN(4K) : AT(ADDR(.low.rodata) + low_disp) ALIGN(4K) {
                *(.low.rodata)
                *(.low.rodata.*)
        }
        .low.data ALIGN(4K) : AT(ADDR(.low.data) + low_disp) ALIGN(4K) {
                *(.low.data)
                *(.low.data.*)
        }
        .low.bss ALIGN(4K) : AT(ADDR(.low.bss) + low_disp) ALIGN(4K) {
                *(.low.dynbss)
                *(.low.bss)
                *(.low.bss.*)
        }
        _low_end = . + low_disp;

        . += low_disp;

        .stack ALIGN (4K) : ALIGN (4K) {
                _stack_start = .;
                . += 16K;
                _stack_end = .;
        }
}
