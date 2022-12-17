#include "miniprintf.h"
#include <stdarg.h>
#include <stdio.h>

char *show_binary(int width, int n) {
  static size_t count = (sizeof n) * 8;
  count = width < count ? width : count;
  static char res[((sizeof n) * 8) + 1];
  for (int i = 0; i < count; i++) {
    res[i] = '0' | ((n >> (count - 1 - i)) & 1);
  }
  res[count] = 0;
  return res;
}

void foo_putc(char ch) { putchar(ch); }

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
  return 0;
}
