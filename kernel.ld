/* This a basic kernel linker script. 
 * It defines boot memory section as an entry point to the OS program.
 * It defines memory layout starting at address 0x8020000 as 
 * sections: .text, .rodata, .data and .bss 
 * Also, it allocates stack space for the program and sets 
 * __bss, __bss_end and __stack_top values. */


/* Entry point of the kernel is boot section */
/* Here, the first instruction to be executed in a program is
 * defined. If -e command line option is not passed, the linker will
 * use ENTRY command as an entry point */
ENTRY(boot)

/* SECTIONS command tells the linker how to map input sections 
 * into output sections, and how to place them into a memory */
SECTIONS {
	
	/* The base address */
	/* This command sets location counter to address 0x8020000
	 * so that the following section (.text) is placed at that address */
	. = 0x80200000;

	/* Section .text.boot is always at the beggining */
	/* .text section contains the code of the program  */
	.text :{
		KEEP(*(.text.boot));
		*(.text .text.*);
	}

	/* .rodata section contains read-only constant data */
	/* This section will be placed immediately after the .text section,
	 * since location counter was not updated manually */
	.rodata : ALIGN(4) {
		*(.rodata .rodata.*);
	}

	/* .data section contains read/write data */
	.data : ALIGN(4) {
		*(.data .data.*);
	}

	/* .bss section contains read/write data that is initially zero valued */
	.bss : ALIGN(4) {
		/* Set __bss variable to the current value of location coutner,
		 * this value is later available outside using extern command */
		__bss = .;
		*(.bss .bss.* .sbss .sbss.*);
		/* Same as __bss, remeber the address at the end of .bss section */
		__bss_end = .;
	}

	/* Align location coutner to 0x04 */
	. = ALIGN(4);
	/* Allocate 128kB of memory for stack */
	. += 128 * 1024; /* 128kB */
	/* Set top of the stack address as a current location counter value,
	 * this value will be available outside using extern command */
	__stack_top = .;
}
