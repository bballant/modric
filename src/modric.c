#include "alvarez.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    json_demo();
  } else if (argc == 3) {
    if (strcmp(argv[2], "-e2j")) {
      edn2json_pprint(argv[2]);
    } else {
      goto usage;
    }
  } else {
    goto usage;
  }
  return 0;

usage:
  if (argc >= 2) {
    printf("You can't do that yet: %s\n", argv[1]);
  }
  printf("usage: modric -e2j filename.edn\n");
  return 0;
}
