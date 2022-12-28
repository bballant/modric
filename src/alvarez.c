#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "edn_parse.h"
#include "json_pprint.h"
#include "modriclib.h"

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

void edn2json_pprint(const char *edn_file) {
  char *res = m_read_text_file(edn_file);
  cJSON *json = edn_parse(res);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
  }
  char *json_str = json_pprint(json);
  printf("%s\n", json_str);

  // manage memory
  free(res);
  cJSON_Delete(json);
  free(json_str);
}

void json_demo(void) {
  printf("This is the JSON demo.\n");

  // pprinting a regular  JSON file
  printf("pprinting a regular  JSON file\n");
  char *res = m_read_text_file("colors.json");
  cJSON *json = cJSON_Parse(res);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
  }
  char *cool = json_pprint(json);
  printf("%s\n", cool);
  free(res);
  cJSON_Delete(json);
  free(cool);

  printf("\n");
  // parsing an edn file and pprinting it as json
  printf("parsing an edn file and pprinting it as json\n");
  edn2json_pprint("colors.edn");
}
