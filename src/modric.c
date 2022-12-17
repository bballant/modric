#include "miniprintf.h"
#include <stdarg.h>
#include <stdio.h>

char *show_binary(int width, int n) {
  size_t count = (sizeof n) * 8;
  count = width < count ? width : count;
  static char res[((sizeof n) * 8) + 1];
  for (int i = 0; i < count; i++) {
    res[i] = '0' | ((n >> (count - 1 - i)) & 1);
  }
  res[count] = 0;
  return res;
}

void foo_putc(char ch) { putchar(ch); }

void print_some_bits() {
  unsigned int a = 60; /* 60 = 0011 1100 */
  unsigned int b = 13; /* 13 = 0000 1101 */
  int c = 0;

  c = a & b; /* 12 = 0000 1100 */
  printf("Line 1 - Value of c is %03d, %s\n", c, show_binary(8, c));

  c = a | b; /* 61 = 0011 1101 */
  printf("Line 1 - Value of c is %03d, %s\n", c, show_binary(8, c));

  c = a ^ b; /* 49 = 0011 0001 */
  printf("Line 3 - Value of c is %03d, %s\n", c, show_binary(8, c));

  c = ~a; /*-61 = 1100 0011 */
  printf("Line 4 - Value of c is %03d, %s\n", c, show_binary(8, c));

  c = a << 2; /* 240 = 1111 0000 */
  printf("Line 5 - Value of c is %03d, %s\n", c, show_binary(8, c));

  c = a >> 2; /* 15 = 0000 1111 */
  printf("Line 6 - Value of c is %03d, %s\n", c, show_binary(8, c));
}

int my_printf(const char *format, ...) {
  va_list args;
  int rc;

  va_start(args, format);
  rc = mini_vprintf_cooked(foo_putc, format, args);
  va_end(args);
  return rc;
}

int main() {
  // printf() displays the string inside quotation
  for (int i = 0; i < 16; i++) {
    printf("%02d, %08x, %s\n", i, i, show_binary(4, i));
    my_printf("Cool %02d %08x\n", i, i);
    // printf("2, %08x\n", ~i << 16);
    // printf("3, %08x\n", ~i << 16 | i);
  }

  print_some_bits();
  return 0;
}
