#include <stdio.h>
#include <string.h>

#include "arocks.h"
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

static void usage(const char *prog) {
  fprintf(stderr,
          "Modric "
          "Usage: %s OPTIONS\n"
          "  -alvarez       - no params, runs demo code\n"
          "  -e2j file.edn  - convert edn to json and pprint it\n"
          "  -ppj file.json - pprint json\n"
          "  -db path-to-db - do something against rocks db at path\n"
          "  -key           - key for rocks db operation (set, get, list)\n"
          "  -value         - value to set at key\n"
          "  -count         - num values to return, starting at key\n",
          prog);
  exit(EXIT_FAILURE);
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

  char *db_path = ".data";
  char *db_key = NULL;
  char *db_value = NULL;
  int db_count = 0;
  int i;

  // Parse command-line flags
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-alvarez") == 0) {
      alvarez_rocks();
      db_path = NULL;
      break;
    } else if (strcmp(argv[i], "-e2j") == 0) {
      edn_to_json_pretty_print(argv[2]);
      db_path = NULL;
      break;
    } else if (strcmp(argv[i], "-ppj") == 0) {
      json_pretty_print(argv[2]);
      db_path = NULL;
      break;
    } else if (strcmp(argv[i], "-db") == 0) {
      db_path = argv[++i];
    } else if (strcmp(argv[i], "-key") == 0) {
      db_key = argv[++i];
    } else if (strcmp(argv[i], "-value") == 0) {
      db_value = argv[++i];
    } else if (strcmp(argv[i], "-count") == 0) {
      db_count = atoi(argv[++i]);
    } else {
      usage(argv[0]);
      break;
    }
  }

  if (db_path != NULL) {
    if (db_key == NULL) {
      usage(argv[0]);
    } else if (db_value != NULL) {
      a_rocks_insert(db_path, db_key, db_value);
    } else if (db_count > 0) {
      char *res[db_count];
      int n = a_rocks_iter(db_path, db_key, db_count, res);
      if (n == 0) {
        printf("key not found\n");
      } else {
        for (int i = 0; i < n; i++) {
          printf("%s\n", res[i]);
        }
      }
    } else {
      char *ret = a_rocks_select(db_path, db_key);
      if (ret == NULL) {
        printf("key not found\n");
      } else {
        printf("%s\n", ret);
      }
    }
  }

  return 0;
}
