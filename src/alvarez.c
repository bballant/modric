#include <stdio.h>
#include <string.h>

#include "read_file.h"
#include "jsmn.h"

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
