#include "responses.h"

#include "util.h"

char *jsonPrint(json_object *json_root) {
    int stdout_save;
    char *buffer = (char *) malloc(2048 * sizeof(char));
    memset(buffer, 0, 2048);
    fflush(stdout);
    stdout_save = dup(STDOUT_FILENO);
    freopen("NUL", "a", stdout); 
    setvbuf(stdout, buffer, _IOFBF, 2048); //set buffer to stdout

    json_object_print(json_root, 0, 1, "", 0);
    fflush(stdout);
    freopen("NUL", "a", stdout); 
    dup2(stdout_save, STDOUT_FILENO);
    setvbuf(stdout, NULL, _IONBF, 2048);
    return buffer;
}

char *makeFULLResponse(string cmd, string id, string time, json_object *args) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "cmd", &percent_escapes, (char*) "result");
    json_object_append_string(json_root, "id", &percent_escapes, (char*)id.c_str());
    json_object_append_string(json_root, "time", &percent_escapes, (char*)time.c_str());
    //json_object_append_string(json_root, "args", &percent_escapes, (char*)result.c_str());
    
    json_object_append_object(json_root, "args", args);
    char *ret = jsonPrint(json_root);
    json_free_object(json_root, 1);
    return ret;
}

json_object *makeMWanIndexResponse(string link, string status) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "link", &percent_escapes, (char*)link.c_str());
    json_object_append_string(json_root, "status", &percent_escapes, (char*)status.c_str());
        
    return json_root;
}

json_object *makeTrasmitResponse(string transmit_host, string latency) {
    LOG
    string succ = "1";
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "host", &percent_escapes, (char*)transmit_host.c_str()); 
    json_object_append_string(json_root, "succ", &percent_escapes, (char*)succ.c_str()); 
    json_object_append_string(json_root, "latency", &percent_escapes, (char*)latency.c_str()); 
    return json_root;
}

json_object *makeDiagResponse(json_object * gw, json_object *resolv, json_object *trasmit_gw, json_object *transmit_host) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_object(json_root, "gw", gw);
    json_object_append_object(json_root, "resolv", resolv);
    json_object_append_object(json_root, "trasmit_gw", trasmit_gw);
    json_object_append_object(json_root, "transmit_host", transmit_host);

    return json_root;
}

json_object *makeResolveResponse(string resolve_host, string resolved_ip, string succ) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "host", &percent_escapes, (char*)resolve_host.c_str()); 
    json_object_append_string(json_root, "success", &percent_escapes, (char*)succ.c_str()); 
    return json_root;
}

json_object *makeGWResponse(string gw_addr, string iface, string addr) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "iface", &percent_escapes, (char*)iface.c_str()); 
    json_object_append_string(json_root, "gw", &percent_escapes, (char*)gw_addr.c_str()); 
    json_object_append_string(json_root, "addr", &percent_escapes, (char*)addr.c_str()); 
    return json_root;
}


json_object *makePortsResponse(string info) {
    LOG
    //printf("makePortsResponse info: %s\n", info.c_str());
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "ports", &percent_escapes, (char*)info.c_str()); 
    return json_root;
}

json_object *makeUnknownResponse() {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "error", &percent_escapes, "method not found");        
    return json_root;
}

json_object *makeMWanResponse(json_object * w1, json_object *w2, string policy) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "policy", &percent_escapes, (char*)policy.c_str());

    json_object_append_object(json_root, "wan" , w1);
    json_object_append_object(json_root, "wan2" , w2);
        
    return json_root;
}

json_object *makeMultiWanResponse(string iface, string mode, string ipaddr, string mask, string hwaddr) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "iface", &percent_escapes, (char*) iface.c_str());
    json_object_append_string(json_root, "mode", &percent_escapes, (char*)mode.c_str());
    json_object_append_string(json_root, "mask", &percent_escapes, (char*)mask.c_str());
    json_object_append_string(json_root, "hwaddr", &percent_escapes, (char*)hwaddr.c_str());
    json_object_append_string(json_root, "ipaddr", &percent_escapes, (char*)ipaddr.c_str());
        
    return json_root;
};

json_object *makeIfaceResponse(string iface, string mode, string ipaddr, string mask, string hwaddr) {
    LOG
    json_object *json_root = json_new_object();

    if(NULL == json_root) {
        printf( "Failed to create new json object\n");
        return NULL;
    }

    json_object_append_string(json_root, "iface", &percent_escapes, (char*) iface.c_str());
    json_object_append_string(json_root, "mode", &percent_escapes, (char*)mode.c_str());
    json_object_append_string(json_root, "mask", &percent_escapes, (char*)mask.c_str());
    json_object_append_string(json_root, "hwaddr", &percent_escapes, (char*)hwaddr.c_str());
    json_object_append_string(json_root, "ipaddr", &percent_escapes, (char*)ipaddr.c_str());
        
    return json_root;
};
