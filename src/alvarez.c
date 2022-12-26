#include <stdio.h>
#include <string.h>

#include "modriclib.h"
#include "cJSON.h"
#include "json_pprint.h"
#include "edn_parse.h"

/* MADNESS */

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
    // printf("2, %08x\n", ~i << 16);
    // printf("3, %08x\n", ~i << 16 | i);
  }
}

void json_demo(void) {
  printf("This is the JSON demo.\n");
  char *res = m_read_text_file("colors.json");
  cJSON *json = cJSON_Parse(res);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
  }

  char *cool = json_pprint(json);
  printf("~~~~~~ COOL ~~~~~~~\n%s\n", cool);

  char *really_cool = edn_parse(res);
  printf("~ Really COOL ~~~~~~~\n%s\n", really_cool);
}

int alvarez_main(int argc, char *argv[]) {
  return 0;
}
