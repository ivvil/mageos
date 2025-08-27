#include <stddef.h>
#include <stdint.h>

/* #if defined(__linux__) */
/* 	#error "This code must be compiled with a cross-compiler" */
/* #elif !defined(__i386__) */
/* 	#error "This code must be compiled with an x86-elf compiler" */
/* #endif */

/* Textmode buff */
volatile uint16_t *vga_buff = (uint16_t *)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0f;

void term_init(void) {
  for (int col = 0; col < VGA_COLS; col++) {
    for (int row = 0; row < VGA_ROWS; row++) {
      const size_t idx = (VGA_COLS * row) + col;
      vga_buff[idx] = ((uint16_t)term_color << 8) |
                      ' '; // Clear screen (set to space char) and set bg and fg
    }
  }
}

void term_putc(char c) {
  switch (c) {
    // Handle newlines
  case '\n': {
    term_col = 0;
    term_row++;
    break;
  }
  default: {
    const size_t idx = (VGA_COLS * term_row) + term_col;
    vga_buff[idx] = ((uint16_t)term_color << 8) | c;
    term_col++;
    break;
  }
  }

  // Handle screen edges
  if (term_col >= VGA_COLS) {
    term_col = 0;
    term_row++;
  }

  if (term_row >= VGA_ROWS) {
    term_col = 0;
    term_row = 0;
  }
}

void term_print(const char *str) {
  for (size_t i = 0; str[i] != '\0'; i++) {
    term_putc(str[i]);
  }
}

void kmain(void) {
  term_init();

  term_print("Hello, World!\n");
}    
