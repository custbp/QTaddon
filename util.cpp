#include "util.h"






std::string intToString(int i)
{
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();

    return s;
};


// int hostname_to_ip(char *hostname , char *ip)
// {
//     int sockfd;  
//     struct addrinfo hints, *servinfo, *p;
//     struct sockaddr_in *h;
//     int rv;
 
//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = SOCK_STREAM;
 
//     if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) 
//     {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//         return 0;
//     }
 
    
//     unsigned int hh_addr;

//     for(p = servinfo; p != NULL; p = p->ai_next) 
//     {
        
//         h = (struct sockaddr_in *) p->ai_addr;
// #ifdef MIPS
//         hh_addr = (unsigned int) h;
// #endif
//         char addr[64] = {0};
//         inet_ntop(AF_INET, &hh_addr, addr, INET_ADDRSTRLEN);
//         //printf("ADDR: %s\n", addr);
//         strcpy(ip , addr );
//     }
     
//     freeaddrinfo(servinfo); 
//     return 1;
// };

// unsigned short
// csum (unsigned short *buf, int nwords)
// {
//   unsigned long sum;
//   for (sum = 0; nwords > 0; nwords--)
//     sum += *buf++;
//   sum = (sum >> 16) + (sum & 0xffff);
//   sum += (sum >> 16);
//   return ~sum;
// };

