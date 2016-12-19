#include "inc.h"
#include "util.h"
typedef struct 
{
    struct nlmsghdr nlMsgHdr;
    struct rtmsg rtMsg;
    char buf[1024];
} route_request;

typedef struct RouteInfo
{
    unsigned long dstAddr;
    unsigned long mask;
    unsigned long gateWay;
    unsigned long flags;
    unsigned long srcAddr;
    unsigned char proto;
    char ifName[IF_NAMESIZE];
};

int hostname_to_ip(char *hostname , char *ip);
int get_iface_addr(char *iface_name, char *setme_ipaddr);
unsigned long ping_host(const char *host, char *from_ip);
void tracert_host(const char *host, char *from_ip);
int findDefaultGw(struct RouteInfo *route, int route_size, char *setme_gwiface, char *gw_addr);
int getGwIface(char *gw_addr, char *iface_name, char *iface_addr);
int get_route_table(struct RouteInfo *route);
int ifname(int if_index, char *ifName);
//unsigned short csum (unsigned short *buf, int nwords);
void lua_dhcp_renew(/* wan iface Real name*/);

void apply_wifi();
void wifi_on();
void wifi_off();
std::string wifi_status();
std::string exec_interfaceMultiwanStatus();
std::string stdexec(const char* cmd);


