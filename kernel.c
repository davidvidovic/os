typedef unsigned char uint8_t;
typedef unsigned char uint32_t;
typedef uint32_t size_t;

/* Symbols are defined in kernel.ld script and here we only obtain their addresses */
/* If used only __bss, we'd get only the value at 0th byte, so [] symboles act as a pointer */
/* Since symbol is aligned to 4 bytes, it will be parsed as a memory location? */
extern char __bss[], __bss_end[], __stack_top[];

void *memset(void *buf, char c, size_t n) 
{
	uint8_t *p = (uint8_t *)buf;
	while(--n)
		*p++ = c;
	return buf;
}

void kernel_main(void) 
{
	// Init __bss section with zeros
	memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

	for(;;);
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
