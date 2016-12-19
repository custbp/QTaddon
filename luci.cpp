#include "luci.h"
#include <sstream>
#include <iostream>
#include <string>
#include "util.h"
#include "luaex.h"
#include "system.h"

using namespace std;

LuciController::LuciController() {

};

int LuciController::init() {
    cfg.init();
    return 1;
};

int LuciController::wifiON() {
    printf("LuciController::wifiON\n");
    int ret = -1;

    return ret;

}

int LuciController::wifiOFF() {
    printf("LuciController::wifiOFF\n");
    int ret = 1;
    

    return ret;
}


#define NULL_INFO "null"

char *LuciController::get_value(const char *name) {
#ifdef MIPS
   struct uci_context *c;
   struct uci_ptr ptr;
   char *copy_name = strdup (name);
   //printf("STRDUP: %s\n", name);
   c = uci_alloc_context ();
   
   char *ret = NULL;
   
    if ((uci_lookup_ptr(c, &ptr, copy_name, true) != UCI_OK) || (ptr.o==NULL || ptr.o->v.string==NULL)) {
        printf("UCI_ERR lookup\n");
        uci_free_context(c);
        free(copy_name);
        return NULL_INFO;
    }

    

    if(ptr.flags & UCI_LOOKUP_COMPLETE) {
        ret = strdup(ptr.o->v.string);
    } else {
        printf("UCI_ERR incomplete\n");
    }

    uci_free_context(c);
    free(copy_name);
   //printf("SEC: %s\n", p.o->section->type);
   //printf("%s: %s\n", name, p.o->v.string);
   
   return ret;
#endif
};

char * LuciController::getDhcpLocalInfo() {
    LOG

    lua_State* L = lua_script("/cust/script.lua", "dhcp_leases");

    if ( ! L ) {
        printf("FAILED\n");
        return "invalid info";
    }

    //return strdup(tok);
    return new char(1);
}


char *LuciController::getWifiPass() {
    return get_value("wireless.@wifi-iface[0].key");
}

char *LuciController::getWifiName() {
    return get_value("wireless.@wifi-iface[0].ssid");
}


json_object *LuciController::getWan2Info() {
    LOG
    std::string wan_iface = get_value("network.wan2.ifname");
    std::string wan_mode = get_value("network.wan2.proto");
    
    std::string wan_ip = "";
    std::string wan_mask = "";

    if (wan_mode != "dhcp") {
        wan_ip = get_value("network.wan2.ipaddr");
        wan_mask = get_value("network.wan2.netmask");
    }

    std::string wan_macaddr = get_value("network.wan2.macaddr");

    return makeIfaceResponse(wan_iface, wan_mode, wan_ip, wan_mask, wan_macaddr);
};

json_object *LuciController::getWanInfo() {
    LOG
    std::string wan_iface = get_value("network.wan1.ifname");
    std::string wan_mode = get_value("network.wan1.proto");
    
    std::string wan_ip = "";
    std::string wan_mask = "";

    if (wan_mode != "dhcp") {
        wan_ip = get_value("network.wan1.ipaddr");
        wan_mask = get_value("network.wan1.netmask");
    }

    std::string wan_macaddr = get_value("network.wan1.macaddr");

    return makeIfaceResponse(wan_iface, wan_mode, wan_ip, wan_mask, wan_macaddr);
};


json_object *LuciController::getLanInfo() {
    LOG
    std::string lan_iface = get_value("network.lan.ifname");
    std::string lan_mode = get_value("network.lan.proto");
    std::string lan_ipaddr = get_value("network.lan.ipaddr");
    std::string lan_macaddr = get_value("network.lan.macaddr");
    std::string lan_mask = get_value("network.lan.netmask");
    return makeIfaceResponse(lan_iface, lan_mode, lan_ipaddr, lan_mask, lan_macaddr);
};

json_object *LuciController::getMultiwanInfo() {
    LOG;
    //json_object *root = json_new_object();

    std::string mwan_interfaces = exec_interfaceMultiwanStatus();
    std::string policy = "60/40"; 
    //printf("mwan_interfaces: %s\n", mwan_interfaces.c_str());
    int wan_1_status = -1;
    int wan_2_status = -1;
    int wan_1_track = -1;
    int wan_2_track = -1;

    sscanf(mwan_interfaces.c_str(), "wan1 %d %d\nwan2 %d %d", &wan_1_status, &wan_1_track, &wan_2_status, &wan_2_track );
    //printf("SSCANF_WAN. WAN_1: %d WAN_2: %d\n", wan_1_status, wan_2_status);
    
    json_object *wan_1 = makeMWanIndexResponse(intToString(wan_1_status), intToString(wan_1_track));
    json_object *wan_2 = makeMWanIndexResponse(intToString(wan_2_status), intToString(wan_2_track));
    return makeMWanResponse(wan_1, wan_2, policy);
}

json_object *LuciController::getPortsInfo() {
   
    //printf("{ \"ports\": %s }", getPortStatus().c_str());
    return makePortsResponse(getPortStatus());
}

json_object *LuciController::getDiagInfo() {
    LOG
    char gw_addr[32] = {0};
    char iface[32] = {0};
    char addr[32] = {0};
    int ret = getGwIface(gw_addr, iface, addr);
    if (!ret) {
        printf("FAILED GET DIAG\n");
        return NULL;
    }

    json_object *gw_info =  makeGWResponse(gw_addr, iface, addr);
    
    string resolve_succ = "0";
    char *resolve_host = "www.ru";
    char *transmit_host = "8.8.8.8";

    char resolve_ip[64] = {0};
    if ( hostname_to_ip(resolve_host, resolve_ip) ) {
        resolve_succ = "1";
    } 

    json_object *resolve_info = makeResolveResponse(resolve_host, string(resolve_ip), resolve_succ);

    int inet_lat = ping_host(transmit_host, addr);

    json_object *transmit_host_info = makeTrasmitResponse(transmit_host, intToString(inet_lat));    
   
    int gw_lat = ping_host(gw_addr, addr);
    
    json_object *transmit_gw_info = makeTrasmitResponse(gw_addr, intToString(gw_lat));

    return makeDiagResponse(gw_info, resolve_info, transmit_gw_info, transmit_host_info);
}
