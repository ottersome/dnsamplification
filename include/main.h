#include <iostream>
#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h> // Core BSD socket functions and data structures.
#include <netinet/in.h> // AF_INET and AF_INET6 address families and their
                        // corresponding protocol families PF_INET and PF_INET6.
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses.
#include <netdb.h>      // Name resolution

struct sockaddr_in ina;
struct ipheader {
    unsigned char      iph_ihl:4, iph_ver:4;
    unsigned char      iph_tos;
    unsigned short int iph_len;
    unsigned short int iph_ident;
    //    unsigned char      iph_flag;
    unsigned short int iph_offset;
    unsigned char      iph_ttl;
    unsigned char      iph_protocol;
    unsigned short int iph_chksum;
    unsigned int       iph_sourceip;
    unsigned int       iph_destip;
};

struct udpheader{
    unsigned short int udph_srcport;
    unsigned short int udph_destport;
    unsigned short int udph_len;
    unsigned short int udph_chksum;
};
struct dnsheader{
    //Copied to corresponding reply. Used to match replies to quieries
    unsigned short int query_id;
    /********************************
     * Flags
    ********************************/
    unsigned char recurs        :1;
    unsigned char trnctdMsg     :1;
    unsigned char authAns       :1;
    unsigned char opCode        :4;
    unsigned char qrFlag        :1;

    unsigned char resCode       :4;
    unsigned char zReserved     :3;
    unsigned char recAvailable  :1;
    //unsigned char checkDsbld    :1;
    //unsigned char authData      :1;
    /********************************
     * End of Flags
    ********************************/
    unsigned short int QDCOUNT;
    unsigned short int ANCOUNT;
    unsigned short int NSCOUNT;
    unsigned short int ARCOUNT;
};
struct dnsQuestions{
    //unsigned char * qname; //not sure about this one
    unsigned short int qtype; 
    unsigned short int qclass; 

};
//struct endOfData{
//    unsigned short int type;
//    unsigned short int dclass;
//};
//struct rdata{
//    unsigned short type;
//    unsigned short _class;
//    unsigned int ttl;
//    unsigned short data_len;
//};
struct dnsAns{
    unsigned short int type;
    unsigned short int aclass;
    unsigned int ttl;
    unsigned short int rdlength;
    unsigned short int nothing;
    unsigned int rdata;//Since im assuming A records
    //RData goes here;
};
struct records{
    unsigned char *name;
    struct rdata *resources;
    unsigned char * rdata;
};
//struct ethhdr{
//    unsigned char h_dest[ETH_ALEN];
//    unsigned char h_dest[ETH_ALEN];
//    __be16        h_proto;
//};
void fillDnsHeader(struct dnsheader * dns, struct dnsQuestions * dnsq);
void chngToDnsFormat(unsigned char* res, unsigned char* ori_host );
void getDnsServers();
