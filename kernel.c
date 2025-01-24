#include "kernel.h"
#include "common.h"

/* Symbols are defined in kernel.ld script and here we only obtain their addresses */
/* If used only __bss, we'd get only the value at 0th byte, so [] symboles act as a pointer */
/* Since symbol is aligned to 4 bytes, it will be parsed as a memory location? */
extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(
		long arg0, 
		long arg1,
		long arg2,
		long arg3,
		long arg4,
		long arg5,
		long fid,
		long eid)
{
	/* Place arguments into registers */
	register long a0 __asm__("a0") = arg0;
	register long a1 __asm__("a1") = arg1;
	register long a2 __asm__("a2") = arg2;
	register long a3 __asm__("a3") = arg3;
	register long a4 __asm__("a4") = arg4;
	register long a5 __asm__("a5") = arg5;
	register long a6 __asm__("a6") = fid;
	register long a7 __asm__("a7") = eid;

	/* By calling ecall instruction, CPU will switch from kernel mode (S-Mode)
	 * to OpenSBI mode (M-Mode). As it reads 0x01 in a7 (EID), it knows that it needs to
	 * print to the console whatever is placed in a0 register. */
	__asm__ __volatile__("ecall"
			: "=r"(a0), "=r"(a1)
			: "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
		        : "memory"	
			);
	return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) 
{
	/* This is sbi_console_putchar function, which uses 0x01 as EID, hence 1 as the last arg */
	sbi_call(ch, 0, 0, 0, 0, 0, 0, 1);
}

void kernel_main(void) 
{
	// Init __bss section with zeros
	memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

	const char *s = "\n\nHello World!";
	for(int i = 0; s[i] != '\0'; i++)
		putchar(s[i]);
	
	printf("\n%s using printf", s);

	const char *s1 = "David";
	char *s2;
	s2 = strcpy(s2, s1);
	printf("\n\n%s %s\n", s1, s2);
	printf("\n\n%d %d", strcmp(s1, s2), strcmp(s1, s));

	for(;;)
		__asm__ __volatile__("wfi");
}


/* 
 * boot section is the entry point, as was defined is kernel.ld linker script
 * Firstly, stack pointer is set to the end address of the stack 
 * Secondly, program jumps to the kernal_main function defined in this file */

__attribute__((section(".text.boot"))) /* this will place boot section at 0x8020000 address */
__attribute__((naked)) /* tells compiler not to generate extra code before and after function body */
void boot(void) 
{
	__asm__ __volatile__ (
		"mv sp, %[stack_top]\n" // set the stack pointer
		"j kernel_main\n"	// jump to the kernel function
		:
		: [stack_top] "r"(__stack_top) // pass the stack top address
	);
}
