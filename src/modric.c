#include "miniprintf.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

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

int my_printf(const char *format, ...) {
  va_list args;
  int rc;

  va_start(args, format);
  rc = mini_vprintf_cooked(foo_putc, format, args);
  va_end(args);
  return rc;
}

void print_binary(int width, int n) { printf("%s", show_binary(width, n)); }

void println_binary(int width, int n) {
  print_binary(width, n);
  printf("\n");
}

void println_binary32(int n) {
  print_binary(32, n);
  printf("\n");
}

void print_some_bits() {
  unsigned int a = 60; /* 60 = 0011 1100 */
  unsigned int b = 13; /* 13 = 0000 1101 */
  int c = 0;

  printf("0 - %s  ", show_binary(8, '0'));
  printf("1 - %s  ", show_binary(8, '1'));

  c = a & b; /* 12 = 0000 1100 */
  printf("%s & ", show_binary(8, a));
  printf("%s ", show_binary(8, b));
  printf("= %03d, %s\n", c, show_binary(8, c));

  c = a | b; /* 61 = 0011 1101 */
  printf("%s | ", show_binary(8, a));
  printf("%s ", show_binary(8, b));
  printf("= %03d, %s\n", c, show_binary(8, c));

  c = a ^ b; /* 49 = 0011 0001 */
  printf("%s ^ ", show_binary(8, a));
  printf("%s ", show_binary(8, b));
  printf("= %03d, %s\n", c, show_binary(8, c));

  c = ~a; /*-61 = 1100 0011 */
  printf("~%s           ", show_binary(8, a));
  printf("= %03d, %s\n", c, show_binary(8, c));

  c = a << 2; /* 240 = 1111 0000 */
  printf("%s << 2       ", show_binary(8, a));
  printf("= %03d, %s\n", c, show_binary(8, c));

  c = a >> 2; /* 15 = 0000 1111 */
  printf("%s >> 2       ", show_binary(8, a));
  printf("= %03d, %s\n", c, show_binary(8, c));
}

static unsigned int ea_thirty[8] = {
    0, 1, 1, 1, 1, 0, 0, 0,
};

int ea_next(int width, int board) {
  int next_board = 0;
  for (int i = 0; i < width; i++) {
    unsigned int neighbor_bits = 0;
    if (i == 0) {
      // left bit = word's rightmost bit
      unsigned int left_bit = (board & 1) << 2; // [100]
      // shift next two bits all the way to right and mask
      unsigned int right_bits = (board >> (width - i - 2)) & 3;
      neighbor_bits = left_bit | right_bits;
    } else if (i == width - 1) {
      // right bit = word's leftmost bit
      unsigned int right_bit = (board >> (width - 1)) & 1;
      unsigned int left_bits = (board & 3) << 1;
      neighbor_bits = left_bits | right_bit;
    } else {
      // neighbor_bits
      neighbor_bits = (board >> (width - i - 2)) & 7;
    }
    // ensure less than 8
    neighbor_bits = neighbor_bits & 7;
    unsigned int c = ea_thirty[neighbor_bits];
    next_board = (next_board << 1) | c;
  }
  return next_board;
}

void elementary_automaton_thirty_(int width, int count) {
  int board = 1 << (width /  2);
  println_binary(width, board);
  for (int i = 0; i < count; i++) {
    board = ea_next(width, board);
    println_binary(width, board);
  }
}

void elementary_automaton_thirty() {
  // 0000000000000001 0000000000000000 = 65536;
  //             0001 0000 in hex
  unsigned int board = 0x10000;
  println_binary32(board);
  static size_t int_size = (sizeof board) * 8;
  for (int j = 0; j < 1000; j++) {
    unsigned int next_board = 0;
    for (int i = 0; i < int_size; i++) {
      unsigned int neighbor_bits = 0;
      if (i == 0) {
        // left bit = word's rightmost bit
        unsigned int left_bit = (board & 1) << 2; // [100]
        // shift next two bits all the way to right and mask
        unsigned int right_bits = (board >> (int_size - i - 2)) & 3;
        neighbor_bits = left_bit | right_bits;
      } else if (i == int_size - 1) {
        // right bit = word's leftmost bit
        unsigned int right_bit = (board >> (int_size - 1)) & 1;
        unsigned int left_bits = (board & 3) << 1;
        neighbor_bits = left_bits | right_bit;
      } else {
        // neighbor_bits
        neighbor_bits = (board >> (int_size - i - 2)) & 7;
      }
      // ensure less than 8
      neighbor_bits = neighbor_bits & 7;
      unsigned int c = ea_thirty[neighbor_bits];
      next_board = (next_board << 1) | c;
    }
    board = next_board;
    println_binary32(board);
  }
}

unsigned int do_mapping(unsigned int n) {
  // only care about bottom 8 bits,
  // and shift one bit to pad for calculation
  unsigned int c = (n & 255) << 1;
  unsigned int next = 0;
  for (int i = 0; i < 8; i++) {
    next |= (ea_thirty[(c >> i) & 3] << i);
  }
  return next;
}

void print_some_stuff() {
  for (int i = 0; i < 16; i++) {
    printf("%02d, %08x, %s\n", i, i, show_binary(4, i));
    my_printf("Cool %02d %08x\n", i, i);
    // printf("2, %08x\n", ~i << 16);
    // printf("3, %08x\n", ~i << 16 | i);
  }
  print_some_bits();

  unsigned int c = 4;
  for (int i = 0; i < 8; i++) {
    c = do_mapping(c);
    printf("%s\n", show_binary(8, c));
  }

  short int i = 30000;
  printf("%d\n", (int)(sizeof i) * 8);
}

int main() {
  elementary_automaton_thirty();
  printf("\n");
  printf("---------------\n");
  printf("\n");
  elementary_automaton_thirty_(4, 100);
  return 0;
}
