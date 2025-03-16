/* This is boot.s file. This is the entry point to the kernel program. */

/* #################################################################### */
/* 
Bootloader will access this code, create constants, labels and symbols, 
memory sections and an executable piece of assembly code defined in this file. 
Memory section .text contains basic x86 instruction that set-up the stack and call kernel program.
*/


/* #################################################################### */
/* Declare constants for the multiboot header */

/* ".set" directive is used to declare constants in assembly code */
.set ALIGN,     1<<0            /* align loaded modules on page bonds */
.set MEMINFO,   1<<1            /* provide memory map */
.set FLAGS,     ALIGN | MEMINFO /* this is the multiboot flag field */
.set MAGIC,     0x1BADB002      /* magic number, lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are in multiboot */
/* Breakdown: ALIGN will have value of 0x1, MEMINFO will have a value of 0x2, FLAGS 0x3 */


/* #################################################################### */
/* Declare .multiboot section */

/* 
".section" is used to specify which section of the program the following instructions, data, or constants should go into
Common sections in assembly are .text (holds programs executable code, ie instructions), 
.data (holds initialized data, eg global vars with init value)
.bss (holds uninitialized data, eg global vars that are not initialized, will be zeroed-out at runtime)
.rodata (holds read-only data, eg constants)
*/
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
/* Breakdown: this creates memory section called "multiboot" and is aligned so that ths start addresses are divisible by 4 */


/* #################################################################### */
/* Define and create an empty stack */

.section .bss
.align 16       /* stack on x86 must be 16-byte aligned */
stack_bottom:   /* create a symbol at the bottom of the stack */
.skip 16384     /* 16 KiB */
stack_top:      /* create a symbol at the top of the stack */
/* Breakdown: in the section .bss that is aligned to 16 bytes, create a symbol for stack bottom, 
then skip 16384 bytes, thus allocating 16KiB of stack space and crate stack_top symbol at the top of the stack */


/* #################################################################### */
/* Define the entry point to the kernel */

.section .text
.global _start /* means the start symbol can be accessed from other files, also by marking it global linker know its the entry point */
.type _start, @function /* just marks the global symbol _start as a function */
_start: /* start of the program */

    /*
	The bootloader has loaded us into 32-bit protected mode on a x86
	machine. Interrupts are disabled. Paging is disabled. The processor
	state is as defined in the multiboot standard. The kernel has full
	control of the CPU. The kernel can only make use of hardware features
	and any code it provides as part of itself. There's no printf
	function, unless the kernel provides its own <stdio.h> header and a
	printf implementation. There are no security restrictions, no
	safeguards, no debugging mechanisms, only what the kernel provides
	itself. It has absolute and complete power over the
	machine.
	*/

    # esp is the stack pointer in x86
    # $ means immediate value, in this case the address of stack_top since it is a symbol
    mov $stack_top, %esp # set-up the stack

    /* After setting up the stack, this is a good place to initialize crucial processor state before the high-level kernel is entered */
    # at this point, will do nothing here


    /*
	Enter the high-level kernel. The ABI requires the stack is 16-byte
	aligned at the time of the call instruction (which afterwards pushes
	the return pointer of size 4 bytes). The stack was originally 16-byte
	aligned above and we've pushed a multiple of 16 bytes to the
	stack since (pushed 0 bytes so far), so the alignment has thus been
	preserved and the call is well defined.
	*/
    call kernel_main


    /*
	If the system has nothing more to do, put the computer into an
	infinite loop. To do that:
	1) Disable interrupts with cli (clear interrupt enable in eflags).
	   They are already disabled by the bootloader, so this is not needed.
	   Mind that you might later enable interrupts and return from
	   kernel_main (which is sort of nonsensical to do).
	2) Wait for the next interrupt to arrive with hlt (halt instruction).
	   Since they are disabled, this will lock up the computer.
	3) Jump to the hlt instruction if it ever wakes up due to a
	   non-maskable interrupt occurring or due to system management mode.
	*/
    cli
1:  hlt
    jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
