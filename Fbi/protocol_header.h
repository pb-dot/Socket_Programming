struct ipReturn {
    uint16_t nextProtocol;
    uint16_t ipHeaderLength;
    uint16_t ipPayloadLength;
};

struct tcpReturn{
    uint16_t dest_port;
    uint16_t src_port;
    uint16_t header_length;
};

////////////////////////////Protocols Supported //////////////////
uint16_t ethernet(FILE * fp, unsigned char * packet);

void arp(FILE * fp, unsigned char * packet);

struct tcpReturn udp(FILE * fp, unsigned char * packet);

struct ipReturn myipv4(FILE * fp, unsigned char * packet);

struct ipReturn myipv6(FILE * fp, unsigned char * packet);

struct tcpReturn mytcp(FILE * fp, unsigned char * packet);

struct tcpReturn icmp(FILE * fp, unsigned char * packet);

