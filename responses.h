#ifndef RESP_H
#define RESP_H

#include "inc.h"


using namespace std;

// extern "C" {
//     #include "jsonutils.h"
// }


const json_escape_pair percent_escape_pairs[] = {
    { L"%", L"%%" },
};

const json_escape percent_escapes = {
    (sizeof(percent_escape_pairs) / sizeof(percent_escape_pairs[0])),
    percent_escape_pairs
};



json_object *makeLanResponse(string iface, string mode, string ipaddr, string mask, string hwaddr);
json_object *makeIfaceResponse(string iface, string mode, string ipaddr, string mask, string hwaddr);

//char *makeLanResponse(string iface, string mode, string ipaddr, string mask, string hwaddr);
//char *makeFULLResponse(string cmd, string id, string time, string result);
char *makeFULLResponse(string cmd, string id, string time, json_object *args);

json_object *makeMWanResponse(json_object * w1, json_object *w2, string policy);
json_object *makeMWanIndexResponse(string link, string status);
json_object *makeUnknownResponse();

json_object *makeGWResponse(string gw_addr, string iface, string addr);
json_object *makeResolveResponse(string resolve_host, string resolved_ip, string succ);
json_object *makeTrasmitResponse(string transmit_host, string latency);

json_object *makeDiagResponse(json_object * gw, json_object *resolv, json_object *trasmit_gw, json_object *transmit_host);

json_object *makePortsResponse(string info);

#endif