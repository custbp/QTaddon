#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"

#include <sys/time.h>
#include "asm/types.h"
#include "linux/netlink.h"
#include "linux/rtnetlink.h"
#include "sys/ioctl.h"
#include "net/if.h"
#include "net/route.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <arpa/inet.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/un.h>

#define SA (struct sockaddr*)
#include <stdexcept>
#include <errno.h>
#include <netdb.h>
#include <vector>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define EX_ERR(msg) throw std::exception(msg " " __FILE__ ":" __LINE__)

// Structure for storing routes
typedef struct RouteInfo
{
    unsigned long dstAddr;
    unsigned long mask;
    unsigned long gateWay;
    unsigned long flags;
    unsigned long srcAddr;
    unsigned char proto;
    bool isDefault;
    char *ifname;
};

typedef struct ResponseInterface {
    std::string if_name;
    std::string response;
};

typedef struct InterfaceInfo {
    char *ip;
    char *mask;
    char *gw;
    bool up;
    bool def;
};

int getIfNameByIndex(int if_index, char **ifName);
int getRouteTable(std::vector<RouteInfo*> &list);
int getIfaceAddr(char *iface_name, char **setme_ipaddr);

InterfaceInfo *makeIfInfoDefault();

#define DEFAULT_IP "-1.-1.-1.-1"
#define DEFAULT_MASK "-1.-1.-1.-1"

#endif //UTIL_H