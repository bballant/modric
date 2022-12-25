#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "cJSON_print.h"

/* define our own boolean type */
#ifdef true
#undef true
#endif
#define true ((cJSON_bool)1)

#ifdef false
#undef false
#endif
#define false ((cJSON_bool)0)

/* strlen of character literals resolved at compile time */
#define static_strlen(string_literal) (sizeof(string_literal) - sizeof(""))
#define cjson_min(a, b) (((a) < (b)) ? (a) : (b))

#define M_INDENT 2

typedef struct internal_hooks {
  void *(*allocate)(size_t size);
  void (*deallocate)(void *pointer);
  void *(*reallocate)(void *pointer, size_t size);
} internal_hooks;

typedef struct {
  unsigned char *buffer;
  size_t length;
  size_t offset;
  size_t depth; /* current nesting depth (for formatted printing) */
  cJSON_bool noalloc;
  internal_hooks hooks;
} printbuffer;

static internal_hooks global_hooks = {malloc, free, realloc};

// in cJSON this might check locale
static unsigned char get_decimal_point(void) { return '.'; }

/* Predeclare these prototypes of functions that call eachother */
static cJSON_bool print_value(const cJSON *const item,
                              printbuffer *const output_buffer);
static cJSON_bool print_array(const cJSON *const item,
                              printbuffer *const output_buffer);
static cJSON_bool print_object(const cJSON *const item,
                               printbuffer *const output_buffer);

/* calculate the new length of the string in a printbuffer and update the offset
 */
static void update_offset(printbuffer *const buffer) {
  const unsigned char *buffer_pointer = NULL;
  if ((buffer == NULL) || (buffer->buffer == NULL)) {
    return;
  }
  buffer_pointer = buffer->buffer + buffer->offset;

  buffer->offset += strlen((const char *)buffer_pointer);
}

/* securely comparison of floating-point variables */
static cJSON_bool compare_double(double a, double b) {
  double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
  return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char *ensure(printbuffer *const p, size_t needed) {
  unsigned char *newbuffer = NULL;
  size_t newsize = 0;

  if ((p == NULL) || (p->buffer == NULL)) {
    return NULL;
  }

  if ((p->length > 0) && (p->offset >= p->length)) {
    /* make sure that offset is valid */
    return NULL;
  }

  if (needed > INT_MAX) {
    /* sizes bigger than INT_MAX are currently not supported */
    return NULL;
  }

  needed += p->offset + 1;
  if (needed <= p->length) {
    return p->buffer + p->offset;
  }

  if (p->noalloc) {
    return NULL;
  }

  /* calculate new buffer size */
  if (needed > (INT_MAX / 2)) {
    /* overflow of int, use INT_MAX if possible */
    if (needed <= INT_MAX) {
      newsize = INT_MAX;
    } else {
      return NULL;
    }
  } else {
    newsize = needed * 2;
  }

  if (p->hooks.reallocate != NULL) {
    /* reallocate with realloc if available */
    newbuffer = (unsigned char *)p->hooks.reallocate(p->buffer, newsize);
    if (newbuffer == NULL) {
      p->hooks.deallocate(p->buffer);
      p->length = 0;
      p->buffer = NULL;

      return NULL;
    }
  } else {
    /* otherwise reallocate manually */
    newbuffer = (unsigned char *)p->hooks.allocate(newsize);
    if (!newbuffer) {
      p->hooks.deallocate(p->buffer);
      p->length = 0;
      p->buffer = NULL;

      return NULL;
    }

    memcpy(newbuffer, p->buffer, p->offset + 1);
    p->hooks.deallocate(p->buffer);
  }
  p->length = newsize;
  p->buffer = newbuffer;

  return newbuffer + p->offset;
}

/* Render the number nicely from the given item into a string. */
static cJSON_bool print_number(const cJSON *const item,
                               printbuffer *const output_buffer) {
  unsigned char *output_pointer = NULL;
  double d = item->valuedouble;
  int length = 0;
  size_t i = 0;
  unsigned char number_buffer[26] = {
      0}; /* temporary buffer to print the number into */
  unsigned char decimal_point = get_decimal_point();
  double test = 0.0;

  if (output_buffer == NULL) {
    return false;
  }

  /* This checks for NaN and Infinity */
  if (isnan(d) || isinf(d)) {
    length = sprintf((char *)number_buffer, "null");
  } else if (d == (double)item->valueint) {
    length = sprintf((char *)number_buffer, "%d", item->valueint);
  } else {
    /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits
     */
    length = sprintf((char *)number_buffer, "%1.15g", d);

    /* Check whether the original double can be recovered */
    if ((sscanf((char *)number_buffer, "%lg", &test) != 1) ||
        !compare_double((double)test, d)) {
      /* If not, print with 17 decimal places of precision */
      length = sprintf((char *)number_buffer, "%1.17g", d);
    }
  }

  /* sprintf failed or buffer overrun occurred */
  if ((length < 0) || (length > (int)(sizeof(number_buffer) - 1))) {
    return false;
  }

  /* reserve appropriate space in the output */
  output_pointer = ensure(output_buffer, (size_t)length + sizeof(""));
  if (output_pointer == NULL) {
    return false;
  }

  /* copy the printed number to the output and replace locale
   * dependent decimal point with '.' */
  for (i = 0; i < ((size_t)length); i++) {
    if (number_buffer[i] == decimal_point) {
      output_pointer[i] = '.';
      continue;
    }

    output_pointer[i] = number_buffer[i];
  }
  output_pointer[i] = '\0';
  output_buffer->offset += (size_t)length;
  return true;
}

/* Render the cstring provided to an escaped version that can be printed. */
static cJSON_bool print_string_ptr(const unsigned char *const input,
                                   printbuffer *const output_buffer) {
  const unsigned char *input_pointer = NULL;
  unsigned char *output = NULL;
  unsigned char *output_pointer = NULL;
  size_t output_length = 0;
  /* numbers of additional characters needed for escaping */
  size_t escape_characters = 0;

  if (output_buffer == NULL) {
    return false;
  }

  /* empty string */
  if (input == NULL) {
    output = ensure(output_buffer, sizeof("\"\""));
    if (output == NULL) {
      return false;
    }
    strcpy((char *)output, "\"\"");

    return true;
  }

  /* set "flag" to 1 if something needs to be escaped */
  for (input_pointer = input; *input_pointer; input_pointer++) {
    switch (*input_pointer) {
    case '\"':
    case '\\':
    case '\b':
    case '\f':
    case '\n':
    case '\r':
    case '\t':
      /* one character escape sequence */
      escape_characters++;
      break;
    default:
      if (*input_pointer < 32) {
        /* UTF-16 escape sequence uXXXX */
        escape_characters += 5;
      }
      break;
    }
  }
  output_length = (size_t)(input_pointer - input) + escape_characters;

  output = ensure(output_buffer, output_length + sizeof("\"\""));
  if (output == NULL) {
    return false;
  }

  /* no characters have to be escaped */
  if (escape_characters == 0) {
    output[0] = '\"';
    memcpy(output + 1, input, output_length);
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
  }

  output[0] = '\"';
  output_pointer = output + 1;
  /* copy the string */
  for (input_pointer = input; *input_pointer != '\0';
       (void)input_pointer++, output_pointer++) {
    if ((*input_pointer > 31) && (*input_pointer != '\"') &&
        (*input_pointer != '\\')) {
      /* normal character, copy */
      *output_pointer = *input_pointer;
    } else {
      /* character needs to be escaped */
      *output_pointer++ = '\\';
      switch (*input_pointer) {
      case '\\':
        *output_pointer = '\\';
        break;
      case '\"':
        *output_pointer = '\"';
        break;
      case '\b':
        *output_pointer = 'b';
        break;
      case '\f':
        *output_pointer = 'f';
        break;
      case '\n':
        *output_pointer = 'n';
        break;
      case '\r':
        *output_pointer = 'r';
        break;
      case '\t':
        *output_pointer = 't';
        break;
      default:
        /* escape and print as unicode codepoint */
        sprintf((char *)output_pointer, "u%04x", *input_pointer);
        output_pointer += 4;
        break;
      }
    }
  }
  output[output_length + 1] = '\"';
  output[output_length + 2] = '\0';

  return true;
}

/* Invoke print_string_ptr (which is useful) on an item. */
static cJSON_bool print_string(const cJSON *const item, printbuffer *const p) {
  return print_string_ptr((unsigned char *)item->valuestring, p);
}

/* Render a value to text. */
static cJSON_bool print_value(const cJSON *const item,
                              printbuffer *const output_buffer) {
  unsigned char *output = NULL;

  if ((item == NULL) || (output_buffer == NULL)) {
    return false;
  }

  switch ((item->type) & 0xFF) {
  case cJSON_NULL:
    output = ensure(output_buffer, 5);
    if (output == NULL) {
      return false;
    }
    strcpy((char *)output, "null");
    return true;

  case cJSON_False:
    output = ensure(output_buffer, 6);
    if (output == NULL) {
      return false;
    }
    strcpy((char *)output, "false");
    return true;

  case cJSON_True:
    output = ensure(output_buffer, 5);
    if (output == NULL) {
      return false;
    }
    strcpy((char *)output, "true");
    return true;

  case cJSON_Number:
    return print_number(item, output_buffer);

  case cJSON_Raw: {
    size_t raw_length = 0;
    if (item->valuestring == NULL) {
      return false;
    }

    raw_length = strlen(item->valuestring) + sizeof("");
    output = ensure(output_buffer, raw_length);
    if (output == NULL) {
      return false;
    }
    memcpy(output, item->valuestring, raw_length);
    return true;
  }

  case cJSON_String:
    return print_string(item, output_buffer);

  case cJSON_Array:
    return print_array(item, output_buffer);

  case cJSON_Object:
    return print_object(item, output_buffer);

  default:
    return false;
  }
}

/* Render an array to text */
static cJSON_bool print_array(const cJSON *const item,
                              printbuffer *const output_buffer) {
  unsigned char *output_pointer = NULL;
  size_t length = 0;
  cJSON *current_element = item->child;

  if (output_buffer == NULL) {
    return false;
  }

  /* Compose the output array. */
  /* opening square bracket */
  length = 2;
  output_pointer = ensure(output_buffer, length + 1);
  if (output_pointer == NULL) {
    return false;
  }
  *output_pointer++ = '[';
  *output_pointer++ = '\n';
  output_buffer->depth++;
  output_buffer->offset += length;

  while (current_element != NULL) {
    /* indent */
    output_pointer = ensure(output_buffer, output_buffer->depth * M_INDENT);
    if (output_pointer == NULL) {
      return false;
    }
    size_t i;
    for (i = 0; i < output_buffer->depth; i++) {
      // output_pointer = add_spaces(2, output_pointer);
      *output_pointer++ = ' ';
      *output_pointer++ = ' ';
    }
    output_buffer->offset += (output_buffer->depth * M_INDENT);

    if (!print_value(current_element, output_buffer)) {
      return false;
    }
    update_offset(output_buffer);
    if (current_element->next) {
      length = 2;
      output_pointer = ensure(output_buffer, length + 1);
      if (output_pointer == NULL) {
        return false;
      }
      *output_pointer++ = ',';
      *output_pointer++ = '\n';
      *output_pointer = '\0';
      output_buffer->offset += length;
    } else {
      output_pointer = ensure(output_buffer, 1);
      if (output_pointer == NULL) {
        return false;
      }
      *output_pointer++ = '\n';
      output_buffer->offset++;
    }
    current_element = current_element->next;
  }

  /* indent the closing brace */
  output_buffer->depth--;
  output_pointer = ensure(output_buffer, (output_buffer->depth * M_INDENT) + 2);
  if (output_pointer == NULL) {
    return false;
  }
  size_t i;
  for (i = 0; i < (output_buffer->depth * M_INDENT); i++) {
    *output_pointer++ = ' ';
  }
  *output_pointer++ = ']';
  *output_pointer = '\0';

  return true;
}

/* Render an object to text. */
static cJSON_bool print_object(const cJSON *const item,
                               printbuffer *const output_buffer) {
  unsigned char *output_pointer = NULL;
  size_t length = 0;
  cJSON *current_item = item->child;

  if (output_buffer == NULL) {
    return false;
  }

  // Print opening brace and then newline
  length = 2; /* fmt: {\n */
  output_pointer = ensure(output_buffer, length + 1);
  if (output_pointer == NULL) {
    return false;
  }
  *output_pointer++ = '{';
  *output_pointer++ = '\n';
  output_buffer->depth++;
  output_buffer->offset += length;

  while (current_item) {

    /* indent */
    output_pointer = ensure(output_buffer, output_buffer->depth * M_INDENT);
    if (output_pointer == NULL) {
      return false;
    }
    size_t i;
    for (i = 0; i < output_buffer->depth; i++) {
      // output_pointer = add_spaces(2, output_pointer);
      *output_pointer++ = ' ';
      *output_pointer++ = ' ';
    }
    output_buffer->offset += (output_buffer->depth * M_INDENT);

    /* print key */
    if (!print_string_ptr((unsigned char *)current_item->string,
                          output_buffer)) {
      return false;
    }
    update_offset(output_buffer);
    length = 2;
    output_pointer = ensure(output_buffer, length);
    if (output_pointer == NULL) {
      return false;
    }
    *output_pointer++ = ':';
    *output_pointer++ = ' ';
    output_buffer->offset += length;

    /* print value */
    if (!print_value(current_item, output_buffer)) {
      return false;
    }
    update_offset(output_buffer);

    /* print comma if not last */
    length = (size_t)(current_item->next ? 2 : 1);
    output_pointer = ensure(output_buffer, length + 1);
    if (output_pointer == NULL) {
      return false;
    }
    if (current_item->next) {
      *output_pointer++ = ',';
    }
    *output_pointer++ = '\n';
    *output_pointer = '\0';
    output_buffer->offset += length;

    current_item = current_item->next;
  }

  /* indent the closing brace */
  output_buffer->depth--;
  output_pointer = ensure(output_buffer, (output_buffer->depth * M_INDENT) + 2);
  if (output_pointer == NULL) {
    return false;
  }
  size_t i;
  for (i = 0; i < (output_buffer->depth * M_INDENT); i++) {
    *output_pointer++ = ' ';
  }
  *output_pointer++ = '}';
  *output_pointer = '\0';

  return true;
}

static unsigned char *m_print(const cJSON *const item,
                              const internal_hooks *const hooks) {
  static const size_t default_buffer_size = 256;
  printbuffer buffer[1];
  unsigned char *printed = NULL;

  memset(buffer, 0, sizeof(buffer));
  /* create buffer */
  buffer->buffer = (unsigned char *)hooks->allocate(default_buffer_size);
  buffer->length = default_buffer_size;
  buffer->hooks = *hooks;
  if (buffer->buffer == NULL) {
    goto fail;
  }

  /* print the value */
  if (!print_value(item, buffer)) {
    goto fail;
  }
  update_offset(buffer);

  /* check if reallocate is available */
  if (hooks->reallocate != NULL) {
    printed =
        (unsigned char *)hooks->reallocate(buffer->buffer, buffer->offset + 1);
    if (printed == NULL) {
      goto fail;
    }
    buffer->buffer = NULL;
  } else /* otherwise copy the JSON over to a new buffer */
  {
    printed = (unsigned char *)hooks->allocate(buffer->offset + 1);
    if (printed == NULL) {
      goto fail;
    }
    memcpy(printed, buffer->buffer,
           cjson_min(buffer->length, buffer->offset + 1));
    printed[buffer->offset] = '\0'; /* just to be sure */

    /* free the buffer */
    hooks->deallocate(buffer->buffer);
  }

  return printed;

fail:
  if (buffer->buffer != NULL) {
    hooks->deallocate(buffer->buffer);
  }

  if (printed != NULL) {
    hooks->deallocate(printed);
  }

  return NULL;
}

/* Render a cJSON item/entity/structure to text. */
CJSON_PUBLIC(char *) m_cJSON_Print(const cJSON *item) {
  return (char *)m_print(item, &global_hooks);
}
