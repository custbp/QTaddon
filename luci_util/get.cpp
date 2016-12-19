#include <uci.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <sstream> //stringstream
#include <iostream> // cout
#include <unordered_map>
#include <stdlib.h> //free
#include "util.h"


typedef std::unordered_map <std::string, std::string> hash_string;

struct uci_context *c = NULL;

std::string intToString(int i)
{
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();

    return s;
}


char *uci_get(const char *str)
{
    //printf("uci_get: %s\n", str);
    struct uci_ptr result = {};
    char str_copy[strlen(str) + 1];
    snprintf(str_copy, (strlen(str) + 1), "%s", str);

    struct uci_context *c = uci_alloc_context ();

    //надо куда нибуьд засунуть
    //uci_unload(c, pkg);  
    //uci_free_context(c);
    //
    if (uci_lookup_ptr(c, &result, (char *) str_copy, true) != UCI_OK) {
        return NULL;
    }

    if (UCI_TYPE_SECTION == result.target && result.s) {
        char *ret = strdup(result.s->type);
        if (!ret)
            printf("Error memory.\n");
        return ret;
    }

    if (!result.o)
        return NULL;

    char *ret = strdup(result.o->v.string);
    if (!ret)
        printf("Error uci get.\n");

    if (!ret)
        ret = strdup("");

    return ret;
}

void findWanInterfaces(hash_string &wanItems) {
    
    struct uci_package * pkg = NULL;  

    c = uci_alloc_context();

    int ret = uci_load(c, "network", &pkg);

    if (ret != UCI_OK){
        printf("FAILED UCI LOAD\n");
        uci_free_context(c);  
        return;
    }

    const char *wan_fmt = "network.wan%d";
    for (int i = 0; i<5; i++) {
        char find_wan_x[32] = "network.wan";
        char wan_x_value[32] = {0};
        
        if (i) sprintf(find_wan_x, wan_fmt, i);
        //printf("CHECK WAN %d exists: %s\n", i, find_wan_x);
        //std::string wan_exist = get_value(find_wan_x);
        //printf("wan_exist: %s\n", wan_exist.c_str());

        char* uci_get_ret = uci_get(find_wan_x);
        if ( uci_get_ret && std::string(uci_get_ret) == "interface" )
        {   
            //wanItems.push_back("wan" + intToString(i));
            wanItems["wan"+intToString(i)] = find_wan_x;
            //printf("FOUND WAN%d\n", i);
        }

        //printf("RETUCIGET: %s\n", uci_get_ret);
        
    }
    uci_unload(c, pkg);  
    uci_free_context(c);  
}


//переписать
int getIfaceSettings(char *ifname, InterfaceInfo **if_info) {
    *if_info = makeIfInfoDefault();
    
    return 1;
};

void getWanInfo(std::string wan_path) {
    printf("getWanInfo: %s\n", wan_path.c_str());
        
    char *ifname = uci_get(std::string(wan_path + ".ifname").c_str());
    if (ifname) printf("ifname: %s\n", ifname);
    //free(ifname);

    char *proto = uci_get(std::string(wan_path + ".proto").c_str());
    if (proto) printf("proto: %s\n", proto);
    free(proto);

    std::string proto_str(proto);

    if ( proto_str == "static" ) {
        char *ipaddr = uci_get(std::string(wan_path + ".ipaddr").c_str());
        if (ipaddr) printf("ipaddr: %s\n", ipaddr);
        free(ipaddr);

        char *netmask = uci_get(std::string(wan_path + ".netmask").c_str());
        if (netmask) printf("netmask: %s\n", netmask);

        free(netmask);
    } else {
        char *ipaddr = NULL;
        int getIfaceAddr_RETURN = getIfaceAddr(ifname, &ipaddr);
        if (ipaddr) printf("ipaddr: %s\n", ipaddr);
        free(ipaddr);
        free(ifname);
    }

    char *macaddr = uci_get(std::string(wan_path + ".macaddr").c_str());
    if (macaddr) printf("macaddr: %s\n", macaddr);
    free(macaddr);

}

int main(int argc, char const *argv[])
{
    printf("main\n");

    hash_string m;
    m["ключ"] = "ЖОПА";
    std::cout << m["ключ"] << std::endl;

    //c = uci_alloc_context ();

    hash_string foundedWanItems;
    findWanInterfaces(foundedWanItems);

    for (auto i = foundedWanItems.begin(); i != foundedWanItems.end(); i++)
    {
        getWanInfo( (*i).second );    
    }
    
    return 0;
}