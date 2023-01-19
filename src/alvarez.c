#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mongoose.h"
#include "alvarez.h"

#define TIME_STR_S 64
#define PAGE "Error!"

// maplewood Lat Long (40.730839, -74.278000)
static const char *s_url = "http://api.open-meteo.com/v1/forecast?latitude=40.73&longitude=-74.28&current_weather=true&temperature_unit=fahrenheit";

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  struct mg_http_serve_opts opts = {.root_dir = "."};   // Serve local dir
  if (ev == MG_EV_HTTP_MSG) mg_http_serve_dir(c, (struct mg_http_message *)ev_data, &opts);
}

static void fn2(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_CONNECT) {
    struct mg_str host = mg_url_host(s_url);
    // Send request
    mg_printf(c,
              "GET %s HTTP/1.0\r\n"
              "Host: %.*s\r\n"
              "\r\n",
              mg_url_uri(s_url), (int) host.len, host.ptr);
  } if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    printf("%.*s", (int) hm->message.len, hm->message.ptr);
  }
}

int serve(uint16_t port) {

  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  char tstr[64];
  strftime(tstr, sizeof(tstr), "%c", tm);
  printf("started at %s\n", tstr);

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  char ip_and_port[13];
  sprintf(ip_and_port, "0.0.0.0:%d", port);
  mg_http_listen(&mgr, ip_and_port, fn, NULL);     // Create listening connection
  mg_http_connect(&mgr, s_url, fn2, NULL);
  for (;;) mg_mgr_poll(&mgr, 1000);                   // Block forever

  // start
  //
  //(void)getc(stdin);
  // stop
  //

  return 0;
}

void alvarez(void) {
  printf("Hello Alvarez!\n");
  serve(8888);
}
