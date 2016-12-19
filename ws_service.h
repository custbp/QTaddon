#ifndef WS_SERVICE_H
#define WS_SERVICE_H

#include "inc.h"
#include "config.h"

#include "luci.h"
#include "util.h"
#include "controller.h"
//#include "frozen.h"

// typedef enum CMD_DIRECTION {
//     GET,
//     SET,
//     ADD,
//     DEL,
//     UPD,
//     RESULT        
// } ;

typedef json_object *(*request_handler)();

class ConnectionService {
public:
    ConnectionService();
    virtual ~ConnectionService();

    virtual void on_connect() = 0;
    virtual void on_disconnect() = 0;
    virtual void on_error() = 0;
    virtual int send(const char *out) = 0;
    virtual void on_receive(const char *in) = 0;
    virtual void loop() = 0;
};

class WSService: public ConnectionService {
private:
    struct lws_context *context;
    struct lws_context_creation_info info;
    struct lws *wsi;
    struct lws_protocols protocol;
    struct lws_protocols client_protocol;
    struct lws_client_connect_info client_info;


    std::string makeFailedResponse(char *err);

protected:
    bool loop_flag;
    bool connected;
    bool connecting;
    int last_connecting_time;
 
    CmdController cmd_ctrl;
    int test;
public:

    // struct method
    // {
    //     CMD_DIRECTION cmd;
    //     const char *  section_name;
    //     //bool          immediate;
    //     request_handler       func;
    // } methods[] =
    // {
    //     //{ GET, ".dhcp", &getDhcpLocalInfo},
    //     //{ GET, ".multiwan", &getMultiwanInfo},
    //     { GET, ".lan", LuciController::getLanInfo},
    //     //{ GET, ".ports", &getPortsInfo},
    //     //{ GET, ".redirect", &getRedirectInfo},
    //     //{ GET, ".wan", &LuciController::getWanInfo},
    //     //{ GET, ".wan2", &LuciController::getWan2Info},
    //     //{ GET, ".wifi", &getWifiInfo},
    //     //{ GET, ".diag", &getDiagInfo2},
        
    //     //{ GET, ".test", &TEST},
        
    //     //{ ADD, ".redirect", &TEST},
    //     //{ DEL, ".redirect", &TEST},
    //     //{ UPD, ".redirect", &TEST},
    // };

    LuciController lc_ctrl;

    WSService();
    ~WSService();

    void on_connect();
    void on_error();
    void on_receive(const char *in);
    void on_disconnect();

    int connect(const char *host, int port, const char *url, const char  *id, const char *proto);
    void reconnect_last();
    int disconnect();

    int send(const char *out);
    
    void loop();
    int init(const char *iface_name, const char *ssl_key, const char *ssl_cert);

    int getGwIface(char *gw_addr, char *iface_name, char *iface_addr);

    std::string parse_cmd_get(std::string args);
    std::string parse_cmd_set(std::string args);

    std::string parseGetLanInfo(std::string args);
    std::string parseGetWanInfo(std::string args);

    int checkJsonErr(int val);
    std::string getDiagInfo();

    void parseSection(std::string in, std::string &section_name, std::vector<std::string> &vals);
    void parseOptions(std::string in);

    void demoJson(std::string in);
};

#endif