#ifndef INC_H
#define INC_H

#include "sys/socket.h"
#include "sys/types.h"
#include <unistd.h>
#include <libwebsockets.h>
#include <sstream>
#include <string>
#include <stdarg.h>

#include <curl/curl.h>

#include <uci.h>
#include <vector>

#ifdef MIPS

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
    #include "jsonutils.h"
}
#endif

#include <map>
#include <iostream>

#include <sys/time.h>
#include "string.h"
#include "asm/types.h"
#include "linux/netlink.h"
#include "linux/rtnetlink.h"
#include "sys/ioctl.h"
#include "net/if.h"
#include "net/route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/un.h>

#define SA (struct sockaddr*)

#include <errno.h>
#include <netdb.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>


#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#endif