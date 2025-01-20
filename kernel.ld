/* Entry point of the kernel is boot section */
ENTRY(boot)

SECTIONS {
	
	/* The base address */
	. = 0x80200000;

	/* Section .text.boot is always at the beggining */

	/* .text section contains the cod of the program  */
	.text :{
		KEEP(*(.text.boot));
		*(.text .text.*);
	}

	/* .rodata section contains read-only constant data */
	.rodata : ALIGN(4) {
		*(.rodata .rodata.*);
	}

	/* .data section contains read/write data */
	.data : ALIGN(4) {
		*(.data .data.*);
	}

	/* .bss section contains read/write data that is initially zero valued */
	.bss : ALIGN(4) {
		__bss = .;
		*(.bss .bss.* .sbss .sbss.*);
		__bss_end = .;
	}

	. = ALIGN(4);
	. += 128 * 1024; /* 128kB */
	__stack_top = .;
}
