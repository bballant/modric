#ifndef MODRICLIB_H_
#define MODRICLIB_H_

#include <stdio.h>
#include <stdlib.h>

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


size_t m_fsize(FILE *fp) {
  long prev = ftell(fp);
  fseek(fp, 0L, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, prev, SEEK_SET); // go back to where we were
  return sz;
}

/*
** Good Enough version for reading text files
** Ensures string ends in '\0'
**
** There are some fancier ways to do this if this doesn't hold up:
** https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
*/
char *m_read_text_file(const char *f_name) {
  FILE *fp = fopen(f_name, "r");
  if (!fp) {
    perror("Error opening file");
    return NULL;
  }
  size_t sz = m_fsize(fp);
  char *buffer = (char *)malloc(sz + 1);
  int c;
  int i = 0;
  while ((c = fgetc(fp)) != EOF) {
    buffer[i++] = (char)c;
  }
  if (ferror(fp)) {
    puts("I/O error reading file");
    buffer = NULL;
  }
  buffer[sz] = '\0';
  fclose(fp);
  return buffer;
}

#endif // MODRICLIB_H_
