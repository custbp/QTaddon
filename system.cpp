#include "system.h"

int hostname_to_ip(char *hostname , char *ip)
{
    int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;
 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
 
    if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
 
    // loop through all the results and connect to the first we can
    unsigned int hh_addr;

    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        
        h = (struct sockaddr_in *) p->ai_addr;
        #ifdef MIPS
        hh_addr = (unsigned int) h;
        #endif
        char addr[64] = {0};
        inet_ntop(AF_INET, &hh_addr, addr, INET_ADDRSTRLEN);
        //printf("ADDR: %s\n", addr);
        strcpy(ip , addr );
    }
     
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

unsigned short
csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

int ifname(int if_index, char *ifName)
{
    int fd,retVal = -1;
    struct sockaddr_in *sin;
    struct ifreq ifr;


    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if(fd == -1)
    {
        perror("socket");
        exit(1);
    }

    ifr.ifr_ifindex = if_index;

    if(ioctl(fd, SIOCGIFNAME, &ifr, sizeof(ifr)))
    {
        perror("ioctl");
        exit(1);
    }

    strcpy(ifName, ifr.ifr_name);
    return 1;
}

int get_route_table(struct RouteInfo *route) {
    int route_sock,i,j;
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

    bzero(request,sizeof(route_request));

    request->nlMsgHdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    request->nlMsgHdr.nlmsg_type = RTM_GETROUTE;
    request->nlMsgHdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;

    request->rtMsg.rtm_family = AF_INET;
    request->rtMsg.rtm_table = 254; 

    if ((retValue = send(route_sock, request, sizeof(route_request), 0)) < 0)
    {
        perror("send");
        exit(1);
    }

    for(;;)
    {
        if( counter < sizeof( struct nlmsghdr))
        {
            printf("Routing table is bigger than 2048\n");
            exit(1);
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
            exit(1);
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
        bzero(&route[i], sizeof(struct RouteInfo));
        flags = rtp->rtm_flags;
        route[i].proto = rtp->rtm_protocol;

        rtap = (struct rtattr *) RTM_RTA(rtp);
        rtl = RTM_PAYLOAD(nlp);
        for( ; RTA_OK(rtap, rtl); rtap = RTA_NEXT(rtap, rtl))
        {
            switch(rtap->rta_type)
            {
                case RTA_DST:
                    count = 32 - rtp->rtm_dst_len;
                    route[i].dstAddr = *(unsigned long *) RTA_DATA(rtap);
                    route[i].mask = 0xffffffff;
                    for (; count!=0 ;count--)
                    route[i].mask = route[i].mask << 1;
                    break;
                case RTA_GATEWAY:
                    route[i].gateWay = *(unsigned long *) RTA_DATA(rtap);
                    break;
                case RTA_PREFSRC:
                    route[i].srcAddr = *(unsigned long *) RTA_DATA(rtap);
                    break;
                case RTA_OIF:
                    ifname(*((int *) RTA_DATA(rtap)),route[i].ifName);
                    break;
                default:
                    break;
            }

        }
        
        route[i].flags|=RTF_UP;

        if (route[i].gateWay != 0)
            route[i].flags|=RTF_GATEWAY;
        if (route[i].mask == 0xFFFFFFFF)
            route[i].flags|=RTF_HOST; 
    }

    return i; 
}

int findDefaultGw(struct RouteInfo *route, int route_size, char *setme_gwiface, char *gw_addr) {
    printf("findDefaultGw START\n" );
    int j = 0;

    for( j = 0; j<=route_size; j++)
    {
        char daddr[64];
        inet_ntop(AF_INET, &route[j].dstAddr, daddr, INET_ADDRSTRLEN);

        char saddr[64];
        inet_ntop(AF_INET, &route[j].gateWay, saddr, INET_ADDRSTRLEN);

        char mask[64];
        inet_ntop(AF_INET, &route[j].mask, mask, INET_ADDRSTRLEN);
        //printf("saddr: %s\n", saddr);

        printf("SHOW ROUTE[%d]: %s %s %s %d %s\n",
            j,
            daddr,
            saddr,
            mask,
            route[j].flags,
            route[j].ifName);
    }

    for( j = 0; j<=route_size; j++)
    {
        char daddr[64];
        inet_ntop(AF_INET, &route[j].dstAddr, daddr, INET_ADDRSTRLEN);

        char saddr[64];
        inet_ntop(AF_INET, &route[j].gateWay, saddr, INET_ADDRSTRLEN);

        char mask[64];
        inet_ntop(AF_INET, &route[j].mask, mask, INET_ADDRSTRLEN);
        //printf("saddr: %s\n", saddr);

        printf("%s %s %s %d %s\n",
            daddr,
            saddr,
            mask,
            route[j].flags,
            route[j].ifName);

        if ( route[j].dstAddr == 0 ) {
            //memcpy(&default_gw, &route[j], sizeof(struct RouteInfo));
            memcpy(setme_gwiface, route[j].ifName, strlen(route[j].ifName));
            memcpy(gw_addr, saddr, strlen(saddr));
            break;
        }
    }
    printf("findDefaultGw END\n" );
}

void tracert_host(const char *host, char *from_ip) {
    //printf("PING FROM: %s TO: %s\n", from_ip, host);

    int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
    char buf[4096] = { 0 };
    struct ip *ip_hdr = (struct ip *) buf;
    int hop = 0;

    int one = 1;
    const int *val = &one;
    if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
        printf ("Cannot set HDRINCL!\n");

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval)) < 0)
        printf ("Cannot set TIMEOUT!\n");

    struct sockaddr_in addr;
    addr.sin_port = htons (7);
    addr.sin_family = AF_INET;
    inet_pton (AF_INET, host, &(addr.sin_addr));

    while (1)
    {
        ip_hdr->ip_hl = 5;
        ip_hdr->ip_v = 4;
        ip_hdr->ip_tos = 0;
        ip_hdr->ip_len = 20 + 8;
        ip_hdr->ip_id = 10000;
        ip_hdr->ip_off = 0;
        ip_hdr->ip_ttl = hop;
        ip_hdr->ip_p = IPPROTO_ICMP;
        inet_pton (AF_INET, from_ip, &(ip_hdr->ip_src));
        inet_pton (AF_INET, host, &(ip_hdr->ip_dst));
        ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);

        struct icmphdr *icmphd = (struct icmphdr *) (buf + 20);
        icmphd->type = ICMP_ECHO;
        icmphd->code = 0;
        icmphd->checksum = 0;
        icmphd->un.echo.id = 0;
        icmphd->un.echo.sequence = hop + 1;
        icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);
        sendto (sfd, buf, 28, 0, SA & addr, sizeof addr);

        char buff[4096] = { 0 };
        struct sockaddr_in addr_recv;
        socklen_t len = sizeof (struct sockaddr_in);
        int recv_ret = recvfrom (sfd, buff, 28, 0, SA & addr_recv, &len);

        struct icmphdr *icmphdr_recv = (struct icmphdr *) (buff + 20);
        if (icmphdr_recv->type != 0) {
            printf ("hop limit:%d Address:%s\n", hop, inet_ntoa (addr_recv.sin_addr));
        }
        else
        {   
            if (recv_ret!=-1) {
                printf ("Reached %s with hop limit:%d\n", inet_ntoa (addr_recv.sin_addr), hop);
                break;
            } else {
                printf("TIMEOUT hop: %d, next\n", hop);
            }
        }

        hop++;
    }

}

unsigned long ping_host(const char *host, char *from_ip) {
    printf("PING FROM: %s TO: %s\n", from_ip, host);

    int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
    char buf[4096] = { 0 };
    struct ip *ip_hdr = (struct ip *) buf;
    int hop = 0;
    int one = 1;
    const int *val = &one;
    if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
        printf ("Cannot set HDRINCL!\n");

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval)) < 0)
        printf ("Cannot set TIMEOUT!\n");

    struct sockaddr_in addr;
    addr.sin_port = htons (7);
    addr.sin_family = AF_INET;
    inet_pton (AF_INET, host, &(addr.sin_addr));

    ip_hdr->ip_hl = 5;
    ip_hdr->ip_v = 4;
    ip_hdr->ip_tos = 0;
    ip_hdr->ip_len = 20 + 8;
    ip_hdr->ip_id = 10000;
    ip_hdr->ip_off = 0;
    ip_hdr->ip_ttl = 64;
    ip_hdr->ip_p = IPPROTO_ICMP;
    inet_pton (AF_INET, from_ip, &(ip_hdr->ip_src));
    inet_pton (AF_INET, host, &(ip_hdr->ip_dst));
    ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);

    struct icmphdr *icmphd = (struct icmphdr *) (buf + 20);
    icmphd->type = ICMP_ECHO;
    icmphd->code = 0;
    icmphd->checksum = 0;
    icmphd->un.echo.id = 0;
    icmphd->un.echo.sequence = hop + 1;
    icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);

    struct timeval tp_begin;
    gettimeofday(&tp_begin, NULL);
    long int ms_start = tp_begin.tv_sec * 1000 + tp_begin.tv_usec / 1000;
    long int ms_end = ms_start;

    //printf("START PING: %d\n", ms_start);
    sendto (sfd, buf, 28, 0, SA & addr, sizeof addr);

    char buff[4096] = { 0 };
    struct sockaddr_in addr_recv;
    socklen_t len = sizeof (struct sockaddr_in);
    int recv_ret = recvfrom (sfd, buff, 28, 0, SA & addr_recv, &len);

    if (recv_ret==-1) {
        printf ("TIMEOUT PING END: %s\n", host);
        return -1;
    }

    struct icmphdr *icmphdr_recv = (struct icmphdr *) (buff + 20);
    if (icmphdr_recv->type == 0) {
        struct timeval tp_end;
        gettimeofday(&tp_end, NULL);
        ms_end = tp_end.tv_sec * 1000 + tp_end.tv_usec / 1000;
        printf("END PING: %s => %d\n", inet_ntoa (addr_recv.sin_addr), ms_end - ms_start);
        return ms_end - ms_start;
    }
    return -1;
    //printf("LATENCY: %d\n", ms_end - ms_start);
}

int get_iface_addr(char *iface_name, char *setme_ipaddr) {
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, iface_name, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    struct sockaddr_in* ipaddr = (struct sockaddr_in *) &ifr.ifr_addr;
    inet_ntop(AF_INET, &ipaddr->sin_addr, setme_ipaddr, INET_ADDRSTRLEN);
    return 1;
};



int getGwIface(char *gw_addr, char *iface_name, char *iface_addr) {
    RouteInfo route[24];

    int num_routes = get_route_table(&route[0]);
    if (!num_routes) {
        printf("ROUTES NOT FOUND\n");
        return 0;
    }
    
    int findgw_ret = findDefaultGw(&route[0], num_routes, iface_name, gw_addr);

    if (!findgw_ret) {
        printf("DEFAULT GW NOT FOUND\n");
        return 0;
    }

    int ifaddr_ret = get_iface_addr( iface_name, iface_addr);

    if ( !ifaddr_ret ) {
        printf("iface addr not found\n");
        return 0;
    }

    return 1;
};


void lua_dhcp_renew(/* wan iface Real name*/) {   
    stdexec("kill -USR1 `ps w | grep udhcpc | grep eth0.3 | grep -v grep | awk '{print $1}'`");
}




void apply_wifi() {
   system ("/sbin/wifi");
}

void wifi_on() {
   system ("/sbin/wifi up");
}

void wifi_off() {
   system ("/sbin/wifi down");
}

std::string wifi_status() {
   return stdexec("/sbin/wifi status");
}

std::string exec_interfaceMultiwanStatus() {
    return stdexec("/usr/sbin/mwan3 interfaces");
}


std::string stdexec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        printf("popen failed\n");;
        return "";
    }

    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
};