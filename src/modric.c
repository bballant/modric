#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "edn_parse.h"
#include "json_pprint.h"
#include "modriclib.h"

void edn_to_json_pretty_print(const char *edn_file) {
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

void json_pretty_print(const char *json_file) {
  char *res = m_read_text_file(json_file);
  cJSON *json = cJSON_Parse(res);
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
  json_pretty_print("colors.json");

  printf("\n");

  // parsing an edn file and pprinting it as json
  printf("parsing an edn file and pprinting it as json\n");
  edn_to_json_pretty_print("colors.edn");
}


int main(int argc, char *argv[]) {
  if (argc == 1) {
    json_demo();
  } else if (argc == 3) {
    if (strcmp(argv[1], "-e2j") == 0) {
      edn_to_json_pretty_print(argv[2]);
    } else if (strcmp(argv[1], "-ppj") == 0) {
      json_pretty_print(argv[2]);
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
