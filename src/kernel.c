#include "limine.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    fb_req = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf(void) {
  for (;;) {
    asm("hlt");
  }
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
  uint8_t *restrict pdest = (uint8_t *restrict)dest;
  const uint8_t *restrict psrc = (const uint8_t *restrict)src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }

  return dest;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *)s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}

void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }
  } else if (src < dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = (const uint8_t *)s1;
  const uint8_t *p2 = (const uint8_t *)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

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
  /* term_init(); */

  /* term_print("Hello, World!\n"); */

  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  if (fb_req.response == NULL || fb_req.response->framebuffer_count < 1) {
    hcf();
  }

  struct limine_framebuffer *fb = fb_req.response->framebuffers[0];

  for (size_t i = 0; i < 100; i++) {
    volatile uint32_t *fb_ptr = fb->address;
    fb_ptr[i * (fb->pitch / 4) + i] = 0xffffff;
  }

  hcf();
}
