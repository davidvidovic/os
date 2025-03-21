/* This is kernel.c file. This is the implementation of BareBonse OS kernel. */

/* 
In this file function kernel_main is defined. This function is called from boot.s file 
and respresents kernel as a program (or a function). Everything releted to the OS kernel 
is called from this function.
*/


/* 
There is no hosted enviroment, and this kernel is developed using freestanding version of the compiler.
This means that no standard C library is provided, however header files are provided by the compiler and
contain only DECLARATION of some important datatypes and data structures
*/
#include <stdbool.h>	// bool datatype
#include <stddef.h>		// size_t, NULL
#include <stdint.h>		// uint8_t, uint16_t, ...
/* Note: using for example int16_t instead of short is very important, since int16_t will alwyas be 16 bits, 
but short can change width and crash entire OS. */

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


/* 
VGA text mode is being used to print out stuff to the terminal. 
Code itself is taken from the OS tutorial but for understanding of the functions 
and things done in here it is necessary to study VGA text mode docs.
I used Wikipedia page: https://en.wikipedia.org/wiki/VGA_text_mode
*/

/* Hardware text mode color constants. */
/* This is taken from a standard for a 16-bit VGA color palette, see wiki page */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* Each screen character is represented by 2 bytes: lower byte is the code point (thing being printed)
and a higher byte contains information about text and background color */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) color << 8 | (uint16_t) uc;
}

/* Sets up the higher byte, fg = foreground, bg = background, see wiki page*/
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return bg << 4 | fg;
}

/* There is no standard C library, so no strlen. Implement one yourself */
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

/* These dimensions are not chosen by accident. See wiki page, paragraph Modes and timings */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	/* Most imporant thing about VGA stuff. VGA buffer is located at memory address 0xB8000 */
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			// Init empty terminal
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void scroll_terminal()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
		}
	}

	terminal_column = 0;
	terminal_row = VGA_HEIGHT-1;
}

void terminal_putchar(char c) 
{
	switch (c)
	{
	case '\n':
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			scroll_terminal();
		break;
	
	default:
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				scroll_terminal();
		}
		break;
	}
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();

	terminal_writestring("Hello, kernel World!\n");
	terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
	terminal_writestring("David is my father.\n");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}