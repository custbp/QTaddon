#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <stdio.h>

static std::string cbi_submit = "cbi.submit";
static std::string tab_wireless_radio0 = "tab.wireless.radio0";
static std::string cbid_wireless_radio0_toggle = "cbid.wireless.radio0.__toggle";
static std::string cbid_wireless_radio0_channel = "cbid.wireless.radio0.channel";
static std::string cbid_wireless_radio0_txpower = "cbid.wireless.radio0.txpower";
static std::string cbid_wireless_radio0_hwmode = "cbid.wireless.radio0.hwmode";
static std::string cbid_wireless_radio0_htmode = "cbid.wireless.radio0.htmode";
static std::string cbi_cbe_wireless_radio0_noscan = "cbi.cbe.wireless.radio0.noscan";
static std::string cbid_wireless_radio0_noscan = "cbid.wireless.radio0.noscan";
static std::string cbid_wireless_radio0_country = "cbid.wireless.radio0.country";

class Config {
private:
    std::string _login;
    std::string _pass;
    std::string my_id;
public:
    Config();
    int init();
    void setID(const char *id);
    int print_POST(std::string &makeme);

    std::string makeAuthorizePost(const char *login, const char *pass);
    std::string makeHello();
    inline std::string ID() { return my_id; };
};

#endif