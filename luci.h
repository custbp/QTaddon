#ifndef LUCI_H
#define LUCI_H

#include "inc.h"
#include "config.h"
#include "responses.h"


enum {
    UCI_LOOKUP_DONE =     (1 << 0),
    UCI_LOOKUP_COMPLETE = (1 << 1),
    UCI_LOOKUP_EXTENDED = (1 << 2),
} flags;

class LuciController {
public:
    Config cfg;
    LuciController();
    int init();
    
    int wifiON();
    int wifiOFF();
    
    char *get_value(const char *name);


    char *getWifiName();
    char *getWifiPass();
    char * getDhcpLocalInfo();
    json_object *getLanInfo();
    json_object *getWanInfo();
    json_object *getWan2Info();
    json_object *getMultiwanInfo();
    json_object *getDiagInfo();
    json_object *getPortsInfo();
};

#endif