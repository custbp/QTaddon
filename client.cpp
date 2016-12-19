#include <libwebsockets.h>

#include "util.h"
#include "ws_service.h"

//static char errstr[4096];
static char iface[16];
//static int xyi = 0;

void show_help() {
    printf("HELP ME PLEASE!\n");
};

void parse_args(int argc, char **argv) {
  int c;  
  if (argc<=1) {
    show_help();
  }

  opterr = 0;
  while ((c = getopt (argc, argv, "vdi:")) != -1) {
    switch (c) {
        case 'v':
        {
            show_help();
            break;
        }
        case 'd':
        {
            printf("[DEBUG MODE]\n");
            c_debug = 1;
            break;
        }
        case 'i':
        {
            strncpy(iface, optarg, strlen(optarg));
            DEBUG printf("listen on: %s\n", iface);
            break;
        }
    }
  }
};

int main(int argc, char **argv)
{
    printf("MAIN\n");
    memset(iface, sizeof(iface), 0);
    parse_args(argc, argv);

    WSService service;

    bool ssl = 0;
    if (!ssl) {
        DEBUG printf("not SSL connect\n");
        service.init(iface, NULL, NULL);
        service.connect("89.105.153.46", 8092, "/router", "008616020028", "main");
    } else {
        DEBUG printf("ssl connect\n");
        service.init(iface, "client.key" , "client.crt");
        service.connect("89.105.153.46", 5000, "/router", "008616020028", "main");
    }

    DEBUG printf("service loop...\n");

    service.loop();

    return 0;
}