#ifndef JSON_PPRINT_H_
#define JSON_PPRINT_H_

#include "cJSON.h"

/* Render a cJSON item/entity/structure to text. */
char * json_pprint(const cJSON *item);

#endif // JSON_PPRINT_H_
