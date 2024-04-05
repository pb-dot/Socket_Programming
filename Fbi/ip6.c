/*

Version                 :4b
Traffic Class           :1B
Flow Label              :20b
Payload length          :2B
Next header             :1B
Hop Limit               :1B
Source IP6              :16B
Destination IP6         :16B
*/

#include "imports.h"
#include "protocol_header.h"

struct ipReturn myipv6(FILE * fp, unsigned char * packet){

    unsigned char version = packet[0]>>4;
    unsigned char traffic_class = ((packet[0] & 15) << 4)+(packet[1]>>4);
    uint16_t flow_label = ((packet[1] & 15) << 16) + (packet[2]<<8) + packet[3];
    uint16_t payload_length = (packet[4]<<8) + packet[5];
    unsigned char next_header = packet[6];
    unsigned char hop_limit = packet[7];
    unsigned char sip6[16];
    unsigned char dip6[16];
    for(int i = 8;i<24;i++){
        sip6[i-8] = packet[i];
        dip6[i-8] = packet[i+16]; 
    }

    fprintf(fp,"\nNetwork Layer Protocol Analysis of the above packet\n");
    fprintf(fp,"\t:IPv6 Header\n");
    fprintf(fp,"Version : %.2X\n", version);
    fprintf(fp,"Traffic Class: 0x%.2X\n", traffic_class);
    fprintf(fp,"Flow Label : 0x%.4X\n",flow_label);
    fprintf(fp,"Payload Length : %d\n", payload_length);
    fprintf(fp,"Hop Limit : %d\n", hop_limit);
    fprintf(fp,"Protocol opcode of next layer : 0x%.2X\n",next_header);
    
    fprintf(fp,"Source IP Address : ");
    for(int i=0;i<8;i++){
        if(i!=0) fprintf(fp," : %.2x%.2x",sip6[i*2],sip6[i*2+1]);
        else fprintf(fp,"%.2x%.2x",sip6[i*2],sip6[i*2+1]);
    }
    
    fprintf(fp,"\nDestination IP Address ");
    for(int i=0;i<8;i++){
        if(i!=0 && i!=7) fprintf(fp," : %.2x%.2x",dip6[i*2],dip6[i*2+1]);
        else if(i==0) fprintf(fp,"%.2x%.2X",dip6[i*2],dip6[i*2+1]);
        else fprintf(fp," : %.2x%.2x\n",dip6[i*2],dip6[i*2+1]);
    }


    struct ipReturn ret;
    ret.ipHeaderLength = 40;
    ret.ipPayloadLength = payload_length;
    ret.nextProtocol = next_header;
}
