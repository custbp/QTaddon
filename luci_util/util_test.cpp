#include <stdio.h>
#include "util.h"

#define MAX_ROUTE_LIST 32

int main(int argc, char const *argv[])
{
    printf("util_test_main\n");
//обработка ошибок не трогать
try {
//
    ////////////////////////////////////////////////////////////////////
    // int getIfNameByIndex(int if_index, char **ifName);
    char *interfaceName = NULL;
    int interfaceIndex = 2;
    int getIfNameByIndex_RETURN = getIfNameByIndex(interfaceIndex, &interfaceName);
    printf("getIfNameByIndex_RETURN: %d\n", getIfNameByIndex_RETURN);
    if (interfaceName) {
        printf("interfaceName: %s\n", interfaceName);
        free(interfaceName);
        interfaceName = NULL;
    } else {
        printf("getIfNameByIndex is NULL\n");
    }
    ////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////
    // int getRouteTable(std::vector<RouteInfo*> &list);
        //typedef struct RouteInfo
        // {
        //     unsigned long dstAddr;
        //     unsigned long mask;
        //     unsigned long gateWay;
        //     unsigned long flags;
        //     unsigned long srcAddr;
        //     unsigned char proto;
        //     char *ifname;
        // };

    std::vector<RouteInfo*> route_list;

    int getRouteTable_RETURN = getRouteTable(route_list);
    printf("getRouteTable_RETURN: %d\n", getIfNameByIndex_RETURN);
    if (getRouteTable_RETURN) {
        for (auto i = route_list.begin(); i != route_list.end(); i++)
        {
            RouteInfo* info = *(i);

            char daddr[64];
            inet_ntop(AF_INET, &info->dstAddr, daddr, INET_ADDRSTRLEN);

            char saddr[64];
            inet_ntop(AF_INET, &info->gateWay, saddr, INET_ADDRSTRLEN);

            char mask[64];
            inet_ntop(AF_INET, &info->mask, mask, INET_ADDRSTRLEN);

            char gw[64];
            inet_ntop(AF_INET, &info->gateWay, gw, INET_ADDRSTRLEN);

            if (info->isDefault)
                printf(">>>>");
            printf("ROUTE %s %s %s %s %s\n", info->ifname, daddr, mask, saddr, gw);
        }
    } else {
        printf("getRouteTable failed\n");
    }
    ////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////
    // int getIfaceAddr(char *iface_name, char **setme_ipaddr);
    char *ifname = "eth0.2";
    char *if_addr = NULL;
    int getIfaceAddr_RETURN = getIfaceAddr(ifname, &if_addr);
    printf("getIfaceAddr_RETURN: %d\n", getIfaceAddr_RETURN);
    if (getIfaceAddr_RETURN) {
        printf("getIfaceAddr NORMAL: %s\n", if_addr);
    } else {
        printf("getIfaceAddr fail\n");
    }
    ////////////////////////////////////////////////////////////////////

//обработка ошибок не трогать
}
    catch(std::string *reason) {
        std::cout << "EXCEPTION FAILED: " << *reason << std::endl;
    }
//

    return 0;
}