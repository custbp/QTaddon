#ifndef UTIL_H
#define UTIL_H

#include "inc.h"

static int c_debug;
#define LOG printf("=> %s:%d\n", __FUNCTION__, __LINE__);
#define END printf("<= %s:%d\n", __FUNCTION__, __LINE__);
#define DEBUG if (c_debug)

static const char *boundary = "----WebKitFormBoundaryrikyerJR0oaO33da";
static const char *setcookie_tkn = "Set-Cookie: ";
static const char *sysauth_tkn  = "sysauth=";
static const char *stok_tkn  = "stok=";
static const char *base_url = "http://192.168.3.1/cgi-bin/luci/";
static const char *content_type_app = "Content-Type: application/x-www-form-urlencoded";

static const char *username_token = "usename=";
static const char *password_token = "password=";

static const char *ipaddrs_tag = "\"addr\"";
static const char *netmask_tag = "\"netmask\"";
static const char *gwaddr_tag = "\"gwaddr\"";
static const char *proto_tag = "\"proto\"";
static const char *is_up_tag = "\"is_up\"";
static const char *macaddr_tag = "\"macaddr\"";
static const char *splitter_name_val = ":";
static const char *splitter_item = ",";
static const char *blank_item = " ";
static const char *kav = "\"";
static const int max_blank = 3;


std::string intToString(int i);
//unsigned short csum (unsigned short *buf, int nwords);
//int hostname_to_ip(char *hostname , char *ip);


// typedef struct wifi_struct { 
//     char *pass;
//     char *name;
//     char *status;
// };

// typedef struct wan_struct { 
//     char *prov;
//     char *ip;
//     char *mask;   
// };

// static int print_wifi_struct(struct json_out *out, va_list *ap) {
//     struct wifi_struct *p = va_arg(*ap, struct wifi_struct *);
//     return json_printf(out, "{status: %Q, pass: %Q, name: %Q}", p->status, p->pass, p->name);
// }

#endif