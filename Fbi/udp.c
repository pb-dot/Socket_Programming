#include "imports.h"
#include "protocol_header.h"

struct tcpReturn udp(FILE * fp, unsigned char * packet) {
    	    		
	        fprintf(fp,"\nTransport Layer Protocol Analysis of the above Packet\n");
		fprintf(fp,"\t:Udp Header\n");
		fprintf(fp,"Src Port : %d\n",ntohs(*(uint16_t *)(packet + 0)));	
		fprintf(fp,"Dest Port : %d\n",ntohs(*(uint16_t *)(packet + 2)));
		fprintf(fp,"UDP Header + Payload Length : %d\n",ntohs(*(uint16_t *)(packet + 4)));
		fprintf(fp,"CheckSum : %d\n",ntohs(*(uint16_t *)(packet + 6)));


    struct tcpReturn ret;
    ret.src_port = ntohs(*(uint16_t *)(packet + 0));
    ret.dest_port = ntohs(*(uint16_t *)(packet + 2));
    ret.header_length = 8;
    return ret;
}



