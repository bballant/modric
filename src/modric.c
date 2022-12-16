#include <stdio.h>

char *show_binary(int n) {
	char res[32];
	return res;
}

int main() {
  // printf() displays the string inside quotation
  for (int i = 0; i < 16; i++) {
    printf("%02d, %08x, %08x\n", i, i, i);
    // printf("2, %08x\n", ~i << 16);
    // printf("3, %08x\n", ~i << 16 | i);
  }
  return 0;
}
