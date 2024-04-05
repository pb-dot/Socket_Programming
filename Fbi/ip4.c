/*
    Internet Protocol Version 4
    Version:Header Length				:1B(4b+4b)
    Differentiated Services Field		:1B
    Total Length						:2B
    Identification						:2B
    Flags					            :3b
    Fragment Offset		          		:13b
    Time to Live					    :1B
    Protocol (0x06 for TCP)	            :1B
    Header Checksum		                :2B
    Source Address				        :4B
    Destination Address		            :4B
*/

#include "imports.h"
#include "protocol_header.h"

struct ipReturn myipv4(FILE* fp, unsigned char* packet){

    unsigned char v_hl = packet[0];
    unsigned char dsf = packet[1];
    uint16_t totlen = (packet[2]<<8) + packet[3];
    uint16_t identification = (packet[4]<<8) + packet[5];
    uint16_t flags_fragoff = (packet[6]<<8) + packet[7];
    unsigned char ttl = packet[8];
    unsigned char protocol = packet[9];
    uint16_t headerChecksum = (packet[10]<<8) + packet[11];
    
    unsigned char flags = flags_fragoff >> 13;
    uint16_t fragment_offset = flags_fragoff & ((1<<13)-1);
    

    fprintf(fp,"\nNetwork Layer Protocol Analysis of the above packet\n");
    fprintf(fp,"\t:IPv4 Header\n");
    fprintf(fp,"Version : %.2X\n", v_hl>>4);
    fprintf(fp,"Header Length: %d\n", 4*(15&v_hl));
    fprintf(fp,"Differentiated Services Field : 0x%.2X\n",dsf);
    fprintf(fp,"Flags (Reserved Bit, No Fragment, More Fragments) : 0x%.2X\n", flags);
    fprintf(fp,"Fragment Offset : 0x%.4X\n",fragment_offset);
    fprintf(fp,"Total Length : %d\n", totlen);
    fprintf(fp,"Time to Live (TTL) : %d\n",ttl);
    fprintf(fp,"Protocol opcode of next layer : 0x%.2X\n",protocol);
    fprintf(fp,"Header Checksum : %d\n",headerChecksum);
    fprintf(fp,"Source IP Address : %d.%d.%d.%d\n",packet[12],packet[13],packet[14],packet[15]);
    fprintf(fp,"Destination IP Address %d.%d.%d.%d\n",packet[16],packet[17],packet[18],packet[19]);


    struct ipReturn ret;
    ret.ipHeaderLength = 4*(15&v_hl);
    ret.ipPayloadLength = totlen - ret.ipHeaderLength;
    ret.nextProtocol = protocol;

    return ret;
}
