#include "config.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/sockios.h>
#include <linux/igmp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

std::string getMacByIfname(const char *ifname) {
    //std::string ret;
    char my_mac[64] = {0};

    int sock;
    if ( (sock=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ) <0) {
        printf("getMacByIfname FAILED CREATE SOCK \n");
    }

    struct ifreq ifr;
    unsigned char *mac = NULL;
    //char *iface = "eth0";
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , ifname , IFNAMSIZ-1);

    if (0 == ioctl(sock, SIOCGIFHWADDR, &ifr)) {
        mac = (unsigned char *)ifr.ifr_hwaddr.sa_data; 
        sprintf(my_mac, "%.2X%.2X%.2X%.2X%.2X%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
    }

    return std::string(my_mac);
};

Config::Config() {

};

int Config::init() {
    _login.clear();
    _pass.clear();
    //static TODO dyn
    std::string my_id = getMacByIfname("eth0.3");
    printf("CONFIG MY ID: %s\n", my_id.c_str());
    setID(my_id.c_str());
    return 0;
};

void Config::setID(const char *id) {
    my_id = std::string(id);
};

std::string Config::makeHello() {
    std::string ret;
    ret.append("{");
        ret.append("\"id\" : \"");
        ret.append(ID());
        ret.append("\", ");
        ret.append("\"cmd\" : \"hello\", ");
        ret.append("\"time\" : \"");
        char buf[16];
        snprintf(buf, 16, "%lu", time(NULL));

        ret.append(buf);
        ret.append("\"");
    ret.append("}");
    //"{ \"id\" : \"008616020028\", \"cmd\": \"hello\", \"time\" : \"1234567890\"}"
    printf("config::makeHello=> %s\n", ret.c_str());
    return ret;
};
