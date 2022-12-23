#include <stdio.h>
#include <string.h>

#include "miniprintf.h"
#include "read_file.h"
#include "jsmn.h"

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

void my_putc(char ch) { putchar(ch); }

int my_printf(const char *format, ...) {
  va_list args;
  int rc;

  va_start(args, format);
  rc = mini_vprintf_cooked(my_putc, format, args);
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

void elementary_automaton_thirty(int width, int count) {
  int board = 1 << (width / 2);
  println_binary(width, board);
  for (int i = 0; i < count; i++) {
    board = ea_next(width, board);
    println_binary(width, board);
  }
}

void print_some_stuff(void) {
  for (int i = 0; i < 16; i++) {
    printf("%02d, %08x, %s\n", i, i, show_binary(4, i));
    my_printf("Cool %02d %08x\n", i, i);
    // printf("2, %08x\n", ~i << 16);
    // printf("3, %08x\n", ~i << 16 | i);
  }
}



void print_token(jsmntok_t *tok) {
  char *typ;
  switch (tok->type) {
  case JSMN_UNDEFINED:
    typ = "JSMN_UNDEFINED";
    break;
  case JSMN_OBJECT:
    typ = "JSMN_OBJECT";
    break;
  case JSMN_ARRAY:
    typ = "JSMN_ARRAY";
    break;
  case JSMN_STRING:
    typ = "JSMN_STRING";
    break;
  case JSMN_PRIMITIVE:
    typ = "JSMN_PRIMATIVE";
    break;
  default:
    typ = "UNKNOWN";
  }

  printf("Token (%s), [%d, %d], %d\n", typ, tok->start, tok->end, tok->size);
}

int alvarez_main(int argc, char *argv[]) {
  jsmn_parser p;
  jsmntok_t t[128];
  jsmn_init(&p);
  char *res = read_text_file("colors.json");
  int num_toks = jsmn_parse(&p, res, strlen(res), t, 128);
  for (int i = 0; i < num_toks; i++ ) {
    print_token(&t[i]);
  }
//  printf("HOly %s\n", res);
  return 0;
}
