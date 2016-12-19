#include "util.h"

typedef struct 
{
    struct nlmsghdr nlMsgHdr;
    struct rtmsg rtMsg;
    char buf[1024];
} route_request;

int getIfNameByIndex(int if_index, char **ifName)
{
    int fd,retVal = -1;
    struct sockaddr_in *sin;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if(fd == -1)
    {
        perror("socket");
        throw new std::string("create socket fail");
    }

    ifr.ifr_ifindex = if_index;

    if(ioctl(fd, SIOCGIFNAME, &ifr, sizeof(ifr)))
    {
        perror("ioctl");
        close(fd);
        throw new std::string("ioctl fail");
    }

    *ifName = strdup(ifr.ifr_name);
    return 1;
   
}

InterfaceInfo *makeIfInfoDefault() {
    InterfaceInfo *if_info = new struct InterfaceInfo();
    if_info->ip = DEFAULT_IP;
    if_info->mask = DEFAULT_MASK;
    if_info->gw = "";
    if_info->up = 0;
    if_info->def = 0;
    return if_info;
};

int getRouteTable(std::vector<RouteInfo*> &list) {
    int route_sock ,i,j;

    route_request *request = (route_request *)malloc(sizeof(route_request));

    int retValue = -1,nbytes = 0,reply_len = 0;
    char reply_ptr[2048];
    ssize_t counter = 2048;
    int count =0;
    struct rtmsg *rtp;
    struct rtattr *rtap;
    struct nlmsghdr *nlp;
    int rtl;
    
    char* buf = reply_ptr;
    unsigned long bufsize ;

    route_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE); 
    if(route_sock == -1)
    {
        throw new std::string("create socket PF_NETLINK fail");
    }

    bzero(request,sizeof(route_request));

    request->nlMsgHdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    request->nlMsgHdr.nlmsg_type = RTM_GETROUTE;
    request->nlMsgHdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;

    request->rtMsg.rtm_family = AF_INET;
    request->rtMsg.rtm_table = 254; 

    if ((retValue = send(route_sock, request, sizeof(route_request), 0)) < 0)
    {
        throw new std::string("send socket PF_NETLINK fail");
    }

    for(;;)
    {
        if( counter < sizeof( struct nlmsghdr))
        {
            printf("Routing table is bigger than 2048\n");
        }

        nbytes = recv(route_sock, &reply_ptr[reply_len], counter, 0);

        if(nbytes < 0 )
        {
            printf("Error in recv\n");
            break;
        }

        if(nbytes == 0)
            printf("EOF in netlink\n");

        nlp = (struct nlmsghdr*)(&reply_ptr[reply_len]);

        if (nlp->nlmsg_type == NLMSG_DONE)
        {
            break;
        }

        if (nlp->nlmsg_type == NLMSG_ERROR)
        {
            printf("Error in msg\n");
        }

        reply_len += nbytes;
        counter -= nbytes;
    }

    bufsize = reply_len;
    unsigned int flags;
    nlp = (struct nlmsghdr *) buf;

    for(i= -1; NLMSG_OK(nlp, bufsize); nlp=NLMSG_NEXT(nlp, bufsize))
    {
        rtp = (struct rtmsg *) NLMSG_DATA(nlp);
        if(rtp->rtm_table != 254)
        continue;

        i++;

        struct RouteInfo *append_item = new struct RouteInfo();

        list.push_back(append_item);

        bzero(append_item, sizeof(struct RouteInfo));
        flags = rtp->rtm_flags;
        append_item->proto = rtp->rtm_protocol;

        rtap = (struct rtattr *) RTM_RTA(rtp);
        rtl = RTM_PAYLOAD(nlp);
        for( ; RTA_OK(rtap, rtl); rtap = RTA_NEXT(rtap, rtl))
        {
            switch(rtap->rta_type)
            {
                case RTA_DST:
                    count = 32 - rtp->rtm_dst_len;
                    append_item->dstAddr = *(unsigned long *) RTA_DATA(rtap);
                    append_item->mask = 0xffffffff;
                    for (; count!=0 ;count--)
                    append_item->mask = append_item->mask << 1;
                    break;
                case RTA_GATEWAY:
                    append_item->gateWay = *(unsigned long *) RTA_DATA(rtap);                  
                    break;
                case RTA_PREFSRC:
                    append_item->srcAddr = *(unsigned long *) RTA_DATA(rtap);
                    break;
                case RTA_OIF:
                    getIfNameByIndex(*((int *) RTA_DATA(rtap)), &append_item->ifname);
                    break;
                default:
                    break;
            }

        }
        
        if (!append_item->dstAddr) {
            append_item->isDefault=true;
        } else {
            append_item->isDefault=false;
        }

        append_item->flags|=RTF_UP;

        if (append_item->gateWay != 0)
            append_item->flags|=RTF_GATEWAY;
        if (append_item->mask == 0xFFFFFFFF)
            append_item->flags|=RTF_HOST; 
    }

    return i; 
}

int getIfaceAddr(char *iface_name, char **setme_ipaddr) {
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, iface_name, IFNAMSIZ-1);
    

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    struct sockaddr_in* ipaddr = (struct sockaddr_in *) &ifr.ifr_addr;
    char addr[64];
    inet_ntop(AF_INET, &ipaddr->sin_addr, addr, INET_ADDRSTRLEN);
    *setme_ipaddr = strdup(addr);

    return 1;
}

#ifdef MAIN
    #include "util_test.cpp"
#endif