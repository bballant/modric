#ifndef CJSON_PRINT_H_
#define CJSON_PRINT_H_

#include "cJSON.h"

/* Render a cJSON item/entity/structure to text. */
CJSON_PUBLIC(char *) m_cJSON_Print(const cJSON *item);

#endif // CJSON_PRINT_H_
