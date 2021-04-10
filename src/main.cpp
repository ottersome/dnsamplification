//TODO: Need to make sure that the checksum for ip and all the 
//  entries in each header are right, I just coded the structure, need to make sure the
//  data we are passing is valid
#include <netinet/udp.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "main.h"//Structs and prototypes here

#define BUFFSIZE ((int)pow(2,16))
#define DOMNAME "www.luisgarciaz.com"

//Remmember that linux is little-endian 
//  and networks tend to be big endian.
unsigned char * qname;
unsigned char packetBuff[BUFFSIZE];
struct dnsQuestions * dnsquestion;
struct sockaddr_in dest;
struct pseudoheader{};
int qnameStrlen = 0;
std::vector<std::string> dnsAddresses;


//getaddrinfo (related to DNS lookup)
//Firewall->What hids your internal ip addresses from the rest of the world. 
//  They use Network Address Translation. (This process changes local ips to remote ips)
//  Nat also allows helps for us not to run out of IPV4 ips
std::ofstream pckSentBin;
std::ofstream pcktRecvBin;
void printHexTillNBytes(unsigned char * mem,ui nBytes){
    unsigned char * curptr = mem;
    for(int i = 0; i < nBytes;i++,curptr++){
        int hexo = *curptr;
        std::cout << std::hex << "/"<<hexo<<"/";
        curptr++;
    }
    std::cout << std::endl;
}
void print4OctIp(unsigned char * mem){
    std::cout <<"First ip in hex"<<std::endl;
    printHexTillNBytes(mem, 4);
    unsigned char * curptr = mem;
    unsigned int oct;
    for(int i = 0; i < 4;i++){
        oct = (unsigned int)*curptr;
        std::cout << std::dec<<  oct;
        if(i<3)
            std::cout <<".";
        curptr++;
    }
    std::cout << std::endl;
}
void printHexTillNull(unsigned char * mem){
    unsigned char * curptr;
    curptr = mem;
    while(*curptr != 0x00){
        int hexo  = *curptr;
        if(hexo >= 0x41 && hexo <= 0x7a)
            std::cout << *curptr;
        else
            std::cout <<"/"<<std::setfill('0')<< std::setw(2)<<std::hex << hexo<<"/";
        curptr++;
    }
    std::cout <<std::endl;
}
int main(){
    getDnsServers();

    /***********************************
     * Opening files for packets
    ***********************************/
    pckSentBin.open("./sentPacket.bin",std::ofstream::binary |std::ofstream::out);
    pcktRecvBin.open("./recvPacket.bin",std::ofstream::binary |std::ofstream::out);
    //Getting address here?
    inet_pton(AF_INET,"Ip adrress here",&(hostIp.sin_addr));//Old way is with inet_addr

    /***********************************
     * Setting socket up
    ***********************************/
    //Declaring headers and memory allocation
    struct iphdr *ip = (struct iphdr*) packetBuff;
    struct udphdr *udp = (struct udphdr*) (packetBuff + sizeof(struct iphdr));
    struct dnsheader *dns = (struct dnsheader*) &packetBuff[0];
    //char * pseudogram,*data;
    struct pseudoheader psh;
    //ZERO-OUt the packetBuff
    memset(packetBuff,0,BUFFSIZE);

    //Setting socket sockaddr_in info
    //printf("We'll be using the dns addres :: %s",dnsAddresses[2].c_str());

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr("8.8.8.8");
    std::cout << "We have "<<dest.sin_addr.s_addr<<" as dns address"<<std::endl;

    uli dataLength = sizeof(struct dnsheader)+sizeof(struct dnsQuestions)+strlen(DOMNAME)+1;
    //TODO: Not so sure if the functions inside are correct or even the params are right in
    //   their format and stuff so you ahve to check here if you get any error
    /***********************************
     * Filling Headers
    ***********************************/
    fillIpHeader(packetBuff,ip,dataLength,hostIp,dest);
    fillUdpHeader(udp,dataLength-sizeof(struct udphdr));
    fillDnsHeader(dns,dnsquestion);

    /***********************************
     * Settings
    ***********************************/
    //Get socket descriptor
    int sd = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);//file descriptor for socket
    //int sd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);//file descriptor for socketk
    std::cout<<"THis is it"<<std::ees;
    if(sd<0){
        perror("FATAL, error when opening socket:");
        exit(-1);
    }

    std::cout << "Done with our constrction, now sending packet "<<std::endl;
    //Writing our packet to binary file
    pckSentBin.write((char*)packetBuff,BUFFSIZE);
    pckSentBin.close();
    /***********************************
     * Sending Data 
    ***********************************/
    if(sendto(sd,
              &packetBuff[0],
              sizeof(struct dnsheader)+sizeof(struct dnsQuestions)+qnameStrlen+1,
              0,
              (struct sockaddr*)&dest,
              sizeof(dest)) <0)
    {
        std::cout<<"Error while sending"<<std::endl;
    }

    memset(packetBuff,0,BUFFSIZE);
    socklen_t sockLen = (socklen_t)sizeof dest;
    std::cout <<"Receiving answer"<<std::endl;

    int recvFromRes = recvfrom(
            sd,
            (char*)&packetBuff[0],
            BUFFSIZE,
            0,
            (struct sockaddr*)&dest,
            &sockLen);
    std::cout << "Checking validity"<<std::endl;
    if(recvFromRes < 0){
        perror("recvfrom failed");
    }
    std::cout <<"recvFromRes : "<<recvFromRes<<std::endl;
    pcktRecvBin.write((char*) packetBuff,BUFFSIZE);
    //printf("This is what we got:\n%.1000s",packetBuff);

    dns = (struct dnsheader*) &packetBuff[0];
    unsigned char * reader = &packetBuff[sizeof(struct dnsheader) + (qnameStrlen+1) + sizeof(struct dnsQuestions)];

    //std::cout <<"Printing hex of result : \n";
    //for(int i = 0;i<recvFromRes;i++){
    //    std::cout <<std::hex<<static_cast<int>(packetBuff[i])<<std::endl;
    //}


    printf("\nThe response contains : ");
	printf("\nThe response id is %d",ntohs(dns->query_id));
	printf("\nThe packet type  type is %d",dns->qrFlag);
	printf("\n %d Questions",ntohs(dns->QDCOUNT));
	printf("\n %d Answers.",ntohs(dns->ANCOUNT));
	printf("\n %d Authoritative Servers.",ntohs(dns->NSCOUNT));
	printf("\n %d Additional records.\n\n",ntohs(dns->ARCOUNT));

    qname = (unsigned char*) dns + (sizeof(struct dnsheader));
    std::cout << "This is the returning qname:" <<std::endl;
    printHexTillNull(qname);
    dnsAns * dnsans = (dnsAns*)(qname+strlen((const char *)qname)+1);
    std::cout << "The following is the DNS answer \n"<<
    "Type is " <<ntohs(dnsans->type)<<"\n"<<
    "Class is " <<ntohs(dnsans->aclass)<<"\n"<<
    "TTL is " <<std::dec<<ntohs(dnsans->ttl)<<"\n"<<
    "RDLength is " <<ntohs(dnsans->rdlength)<<"\n"<<
    std::endl;
    unsigned int finalIp = ntohs(dnsans->rdata);
    std::cout << "Final Ip is : "<<std::endl;
    print4OctIp((unsigned char*)&finalIp);


    //This might be part of what they gave us
    //udp->uph_srcport = htons(atoi(arv[2]));A//htons: host to network short
    close(sd);
    //pckSentBin.close();
    pcktRecvBin.close();
}
void fillDnsHeader(struct dnsheader * dns, struct dnsQuestions * dnsq){
    //dns->query_id = (unsigned short) htons(getpid());
    unsigned short int ids= 1337;
    dns->query_id = (unsigned short) htons(ids);
    dns->recurs 		= 1; 
    dns->trnctdMsg 		= 0;
    dns->authAns 		= 0;
    dns->opCode 		= 0;
    dns->qrFlag 		= 0;
    dns->resCode        = 0;
    dns->zReserved 		= 0;
    dns->recAvailable 	= 0;

    dns->QDCOUNT        = htons(1);
    dns->ANCOUNT        = 0;
    dns->NSCOUNT        = 0;
    dns->ARCOUNT        = 0;
    //dns->authData 		= 0;
    //dns->checkDsbld 	= 0;
    //memset(&dns[2],1,1);
    //memset(&dns[2],2,1);

    //Now for the query:
    //unsigned char * qname = (unsigned char*) dns + (sizeof(struct dnsheader));
    qname = (unsigned char*) dns + (sizeof(struct dnsheader));
    unsigned char  hostName[100];
    memset(hostName,0,sizeof(hostName));
    strcpy((char*)hostName,DOMNAME);
    chngToDnsFormat(qname,hostName);//This shoudl work fine

    dnsq = (dnsQuestions*)(qname + strlen((const char*)qname)+1);
    qnameStrlen = strlen((const char*) qname);

    dnsq->qtype = htons(0x0001);//Not sure here
    dnsq->qclass = htons(0x0001);


}
void chngToDnsFormat(unsigned char* res,unsigned char* ori_host ){
    int lock = 0 , i;
    strcat((char*)ori_host,".");
    //std::cout << "This is ori_host now : "<<ori_host<<std::endl;
    unsigned char * oriRes  = res;
    for(i = 0 ; i < strlen((char*)ori_host) ; i++) 
    {
        if(ori_host[i]=='.') 
        {
            *res++ = i-lock;
            for(;lock<i;lock++) 
            {
                *res++=ori_host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *res++='\0';
    //std::cout << "Converted ori is  : "<<(char*)oriRes<<std::endl;
}

void getDnsServers(){
    std::ifstream infile("/etc/resolv.conf");
    std::string line;
    std::string token;
    while(std::getline(infile,line)){
        std::istringstream iss(line);
        iss >>token;
        if(token=="nameserver"){
            iss >>token;
            //printf("The next id is : %s\n",token.c_str());
            dnsAddresses.push_back(token);
        }
    }
    for(int i = 0;i<dnsAddresses.size();i++){
        //printf("This is %d, with dnsAddress : %s\n",i,dnsAddresses[i].c_str());
    }

}
unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}
void fillIpHeader(unsigned char * packetPtr, iphdr* ip,uli dataLength,sockaddr_in & source_ip,sockaddr_in & destIpData ){
    ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = sizeof(struct iphdr)+sizeof(struct udphdr) + dataLength/*strlen(data)*/;//Not so sure about this one rigck 
	ip->id = htonl (54321);	//Id of this packet
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_UDP;
	ip->check = 0;		//Set to 0 before calculating checksum
	ip->saddr = source_ip.sin_addr.s_addr;	//Spoof the source ip addresIs
	ip->daddr = destIpData.sin_addr.s_addr;
	//Ip checksum
	ip->check = csum ((unsigned short *) packetPtr, ip->tot_len);
}
void fillUdpHeader(udphdr * udp,uli dataLength){
	udp->source = htons (6666);
	udp->dest = htons (8622);
	udp->len = htons(8 + dataLength/*strlen(data)*/);	//tcp header size
	udp->check = 0;	//leave checksum 0 now, filled later by pseudo header
}



//void parseAnswer(char * readPointer, char * buff){
//    
//    struct dnsheader *dns = (struct dnsheader*) &packetBuff[0];
//    struct records answers[20];
//
//    for(int i = 0 ;i<ntohs(dns->ANCOUNT);i++){
//        answers[i].name = 
//
//    }
//}
//unsigned char * (unsigned char * readPointer, unsigned char * buffer, int* count ){
//
//    unsigned char * properName;
//    while(*readPointer!=0){
//        if(*reader>=192){
//Z a
//        }
//
//    }
//}




    /*
    //Bind stuff
    struct addrinfo hints, *res;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, "3490", &hints, &res);
    int bindres  = bind(sd,res->ai_addr,res->ai_addrlen);
    if(bindres = -1){
        std::cerr << "FATAL: Error binding the socket"<<std::endl;
        return -1;
    }*/
    //Make sure to get the port
    //int yes=1;
    //if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1){
    //    perror("Error at forcing socket binding");
    //    exit(1);
    //}

    //Connecting woo
    //int sendSockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
