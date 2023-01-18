#include "alvarez.h"

#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_STR_S 64
#define PAGE "Error!"


static enum MHD_Result ahc_echo(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **ptr) {
  static int dummy;
  struct MHD_Response *response;
  int ret;

  if (0 != strcmp(method, "GET"))
    return MHD_NO; /* unexpected method */
  if (&dummy != *ptr) {
    /* The first time only the headers are valid,
       do not respond in the first round... */
    *ptr = &dummy;
    return MHD_YES;
  }
  if (0 != *upload_data_size)
    return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL;     /* clear context pointer */

  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  char *tstr = malloc(64 * sizeof(char));
  strftime(tstr, 64 * sizeof(char), "%c", tm);

  response = MHD_create_response_from_buffer(strlen(tstr), (void *)tstr,
                                             MHD_RESPMEM_MUST_FREE);

  if (response == NULL) {
    return MHD_NO;
  }

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  return ret;
}

int serve(uint16_t port) {

  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  char tstr[64];
  strftime(tstr, sizeof(tstr), "%c", tm);
  printf("started at %s\n", tstr);

  struct MHD_Daemon *d;

  d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL,
                       &ahc_echo, PAGE, MHD_OPTION_END);
  if (d == NULL)
    return 1;
  (void)getc(stdin);
  MHD_stop_daemon(d);
  return 0;
}

void alvarez(void) {
  printf("Hello Alvarez!\n");
  serve(8888);
}
