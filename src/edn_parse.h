#ifndef EDN_PARSE_H_
#define EDN_PARSE_H_

#include "cJSON.h"

/* Render a cJSON item/entity/structure to text. */
char * edn_parse(const char *value);

#endif // EDN_PARSE_H_
