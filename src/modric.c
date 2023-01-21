#include <stdio.h>
#include <string.h>

#include "alvarez.h"
#include "alvarez_rocks.h"
#include "cJSON.h"
#include "edn_parse.h"
#include "json_pprint.h"
#include "modriclib.h"
#include "http_client.h"
#include "http_server.h"

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

/*
** ./bin/modric # no params, runs json_demo()
** ./bin/modric -alvarez # run testing code
** ./bin/modric -e2j file.edn  # convert edn to json and pprint
** ./bin/modric -ppj file.json # pprint json
**
**  # rocksdb playin'
**    # insert doc
**  ./bin/modric -db path-to-db -key string-key-for-json -json path-to-json-file
**    # print doc
**  ./bin/modric -db path-to-db -key string-key-for-json
* */
int main(int argc, char *argv[]) {
  if (argc == 1) {
    json_demo();
  } else if (argc == 7 && strcmp(argv[1], "-db") == 0) {
    printf("insert %s into db %s\n", argv[4], argv[2]);
    a_rocks_insert(argv[2], argv[4], argv[6]);
    printf("Success!\n");
  } else if (argc == 5 && strcmp(argv[1], "-db") == 0) {
    printf("fetch %s from db %s\n", argv[4], argv[2]);
    char *ret = a_rocks_select(argv[2], argv[4]);
    printf("result: %s\n", ret);
    free(ret);
  } else if (argc == 5 && strcmp(argv[1], "-db") == 0) {
  } else if (argc == 2 && strcmp(argv[1], "-alvarez") == 0) {
    //alvarez();
    //alvarez_rocks();
    //run_http_client();
    char *server_args[3] = {"foo", "-d", "."};
    run_http_server(3, server_args);
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
