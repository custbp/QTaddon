#include "ws_service.h"
#define MIPS 1

#include "frozen.h"
#include "system.h"
#include "util.h"
#include "luaex.h"

// extern "C" {
//     #include "jsonutils.h"
// }

#include "responses.h"

static int websocket_write_back(struct lws *wsi_in, char *str, int str_size_in) 
{
    if (str == NULL || wsi_in == NULL)
        return -1;

    int n;
    int len;
    unsigned char *out = NULL;

    if (str_size_in < 1) 
        len = strlen(str);
    else
        len = str_size_in;

    out = (unsigned char *)malloc(sizeof(char)*(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
    //* setup the buffer*/
    memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len );
    //* write out*/
    n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);

    DEBUG printf("[websocket_write_back] %s\n", str);
    //* free the buffer*/
    free(out);

    return n;
}

static int ws_service_callback(
                         struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    ConnectionService *service = (ConnectionService*) user;
    int ret = -1;
    switch (reason) {

        case LWS_CALLBACK_CLIENT_ESTABLISHED: //клиент пришел
            printf("[ws_service_callback] Connect with server success.\n");
            service->on_connect();
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: //клиент закрыл НЕправильно
            printf("[ws_service_callback] Connect with server error.\n");
            
            service->on_error();
            break;

        case LWS_CALLBACK_CLOSED: //клиент закрыл правильно
            printf("[ws_service_callback] LWS_CALLBACK_CLOSED\n");
            service->on_disconnect();
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE: //что то пришло
            printf("[ws_service_callback] Client recvived:%s\n", (char *)in);
            service->on_receive((char *)in);
            printf("on_receive -> SEND\n");
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE : //клиенту можно отправить из его очереди
            printf("[ws_service_callback] On writeable is called. send byebye message\n");
            
            break;

        default:
            break;
    }
    
    return 0;
};



static int ws_client_callback(
                         struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    
    switch (reason) {

        case LWS_CALLBACK_CLIENT_ESTABLISHED: //клиент пришел
            printf("[ws_client_callback] Connect with server success.\n");
            
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: //клиент закрыл НЕправильно
            printf("[ws_client_callback] Connect with server error.\n");
            
            break;

        case LWS_CALLBACK_CLOSED: //клиент закрыл правильно
            printf("[ws_client_callback] LWS_CALLBACK_CLOSED\n");
            
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE: //что то пришло
            printf("[ws_client_callback] Client recvived:%s\n", (char *)in);

            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE : //клиенту можно отправить из его очереди
            printf("[ws_client_callback] On writeable is called. send byebye message\n");
           
            break;

        default:
            break;
    }

    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "main",
        ws_service_callback,
        sizeof(ConnectionService),
        0,
    },
    {
        NULL, NULL, 0
    }
};

ConnectionService::ConnectionService() {};
ConnectionService::~ConnectionService() {

};

WSService::WSService() {
    context = NULL;
    memset(&info, 0, sizeof info);
    wsi = NULL;
    memset(&client_info, 0, sizeof(client_info));

};

int WSService::init(const char *iface_name, const char *ssl_key, const char *ssl_cert) {
    DEBUG printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);

    connected  = false;
    connecting  = false;
    lc_ctrl.init();

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.iface = iface_name;
    info.protocols = protocols;
    info.options = 0;
    if (ssl_key == NULL) {
        info.ssl_cert_filepath = NULL;
    } else {
        info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    }
    if (ssl_cert == NULL) {
        info.ssl_private_key_filepath = NULL;
    }



    info.gid = -1;
    info.uid = -1;
    
    //
    loop_flag = 1;

    context = lws_create_context(&info);
    

    if (context == NULL) {
        printf("lws_create_context failed\n");
        return -1;
    }
}

WSService::~WSService() {

};

void WSService::reconnect_last() {
    wsi = lws_client_connect_via_info(&client_info);
    connecting = true;
    last_connecting_time = time(NULL);
}

void WSService::on_error() {
    printf("WSService::on_error\n");
    on_disconnect();
};

int WSService::send(const char *out) {
    printf("WSService::send: %s\n", out);
    char *out_wrap = (char*) out;
    websocket_write_back(wsi, out_wrap, -1);
    printf("ENDOF\n");
};


typedef struct {
    int need_walk;
    json_object *root;
    WSService *self;
} arg_response;

// char* getRedirectList() {
//     char *buff = (char*)malloc(1024);
//     memset(buff, 1024, 0);
//     sprintf(buff, "{ \"name\" : \"%s\" }", "1", "2");
//     return buff;
// }

// char * getRedirectInfo(LuciController &lc, size_t len, const char* tok) {
//     char *buff = (char*) malloc(1024);
//     memset(buff, 1024, 0);
//     char *s = getRedirectList();
//     sprintf(buff, "{ \"rediret\": %s }", s);
//     free(s);
//     return buff;
// }

static void args_walker(
    void *callback_data,
    const char *name, size_t name_len, 
    const char *path,
    const struct json_token *token) 
{
    arg_response *resp = (arg_response*)callback_data;
    json_object *args_root = (json_object*)resp->root;
    WSService *svc = (WSService*)resp->self;

    int *need_walk = &(resp->need_walk);
    
    if (!need_walk) return;
    
    //printf("WALKER: %.*s => %s TYPE: %d PTR: %.*s\n", name_len, name, path, token->type, token->len, token->ptr);
    
    json_object *method_result = NULL;
    if ( token->type == JSON_TYPE_OBJECT_END ) {
        if (!strlen(path))
            return;
        //{ GET, ".lan", LuciController::getLanInfo},
        if ( strcmp(path, ".lan") == 0 ) {
            method_result = svc->lc_ctrl.getLanInfo();
            *need_walk = 0;
        } else if ( strcmp(path, ".wan") == 0 ) {
            method_result = svc->lc_ctrl.getWanInfo();
            *need_walk = 0;
        } else if ( strcmp(path, ".wan2") == 0 ) {
            method_result = svc->lc_ctrl.getWan2Info();
            *need_walk = 0;
        } else if ( strcmp(path, ".multiwan") == 0 ) {
            method_result = svc->lc_ctrl.getMultiwanInfo();
            *need_walk = 0;
        }
        else if ( strcmp(path, ".ports") == 0 ) {
            method_result = svc->lc_ctrl.getPortsInfo();
            *need_walk = 0;
        }
        else if ( strcmp(path, ".diag") == 0 ) {
            method_result = svc->lc_ctrl.getDiagInfo();
            *need_walk = 0;
        } else {
            printf("UNKNOWN PATH: %s\n", path);
            method_result= makeUnknownResponse();
            *need_walk = 0;
        };

        if (method_result)
        json_object_append_object(args_root, (char*)path + 1 , method_result);
    }
    return;
}

void WSService::on_receive(const char *in_json) {
    
    char *id = NULL;
    char *cmd = NULL;
    char *qtime = NULL;
    
    std::string s_id("");
    std::string s_cmd("");
    std::string s_qtime("");

    int json_len = strlen(in_json);

    int id_found = json_scanf(in_json, json_len, "{ id:%Q }", &id);
    int cmd_found = json_scanf(in_json, json_len, "{ cmd:%Q }", &cmd);
    int qtime_found = json_scanf(in_json, json_len, "{ time:%Q }", &qtime);

    if (!id_found)  {
        printf("ID NOT FOUND FAILED!\n");
        //out_response = makeFailedResponse("JOPA");
    }
    
    if (!id_found)  {
        printf("ID NOT FOUND FAILED!\n");
        return;
        //goto fail;
    } else {
        s_id = std::string(id);
        free(id);
        printf("ID: %s\n", s_id.c_str());
        if ( std::string(s_id) != lc_ctrl.cfg.ID() ) {
            printf("FAILED NOT MY ID\n");
            printf("MY ID IS: %s\n", lc_ctrl.cfg.ID().c_str());
        }
    }

    if (!cmd_found)  {
        printf("CMD NOT FOUND FAILED!\n");
    } else {
        s_cmd = std::string(cmd);
        free(cmd);
        printf("CMD: %s\n", s_cmd.c_str());   
    }

    if (!qtime_found)  {
        printf("TIME NOT FOUND FAILED!\n");
    } else {
        s_qtime = std::string(qtime);
        free(qtime);
        printf("TIME: %s\n", s_qtime.c_str());   
    }
    
    if ( s_cmd == "hello_ok") {
        return;
    }
   
    arg_response a_resp;
    a_resp.root = json_new_object();
    a_resp.self = this;

    if ( s_cmd == "get") {
        a_resp.need_walk = 1;
        struct json_token t;
        for (auto i = 0; json_scanf_array_elem(in_json, json_len, ".args", i, &t) > 0; i++) {
            int walk_succ = json_walk(t.ptr, t.len, &args_walker, &a_resp);    
            a_resp.need_walk = 1;            
            break;
        }
    }

    char *fullResponse = makeFULLResponse(cmd, lc_ctrl.cfg.ID(), s_qtime, a_resp.root);
    
    printf("fullResponse: %s\n", fullResponse);
    send(fullResponse); free(fullResponse);

    return;
};


void WSService::on_connect() {
    printf("WSService::on_connect\n");
    std::string hello_msg = lc_ctrl.cfg.makeHello();
    send(hello_msg.c_str());
    connected = true;
    connecting = false;
};

int WSService::connect(const char *host, int port, const char *url, const char  *id, const char *proto) {
    client_protocol.callback = ws_client_callback;

    client_info.context = context;
    client_info.address = host;
    client_info.port = port;
    client_info.ssl_connection = 0;
    client_info.path = url;
    client_info.host = id;
    client_info.protocol = proto;
    client_info.ietf_version_or_minus_one = -1;
    client_info.userdata = this;
    
    wsi = lws_client_connect_via_info(&client_info);
    connecting = true;
    connected = false;
    last_connecting_time = time(NULL);
};

int WSService::disconnect() {

};

void WSService::on_disconnect() {
    printf("WSService::on_disconnect\n");
    connected = false;
    connecting = false;
};

void WSService::loop() {
    while (loop_flag) {
        lws_service(context, 50);
        if (!connected && !connecting && (time(NULL) - last_connecting_time > 10) ) {
            printf("Trying connect ..\n");
            reconnect_last();
        }
    }
};