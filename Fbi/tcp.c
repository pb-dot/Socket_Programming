/*

Transmission Control Protocol
    Source Port						            :2B
    Destination Port					        :2B
    Sequence Number (raw)				        :4B
    Acknowledgment number (raw)	                :4B
    Header Length					            :4b
    Flags					                    :12b
    Window [Window size scaling factor 128]     :2B
    Checksum						            :2B
    Urgent Pointer					            :2B							
    Options					                    :12B
*/

#include "imports.h"
#include "protocol_header.h"

uint32_t byteToNum(unsigned char* packet, int i, int size){

    uint32_t ans = packet[i];
    for(int j=i+1;j<i+size;j++){
        ans = (ans<<8) + packet[j];
    }
    return ans;
}

struct tcpReturn mytcp(FILE* fp, unsigned char* packet){

    uint16_t src_port = (uint16_t)byteToNum(packet,0,2);
    uint16_t dest_port = (uint16_t)byteToNum(packet,2,2);
    uint32_t sequence_num = byteToNum(packet,4,4);
    uint32_t ack_num = byteToNum(packet,8,4);
    uint16_t header_length = 4*(packet[12]>>4);
    uint16_t flags = ((packet[12] & 15)<<4) + packet[13];
    uint16_t window_size = ((packet[14])<<8 + packet[15]);
    uint16_t checksum =(uint16_t) byteToNum(packet,16,2);
    uint16_t urgent_pointer= (uint16_t)byteToNum(packet,18,2);
    unsigned char options[40];
    if(header_length>20){
        
        for(int i=0;i<header_length-20;i++){
            options[i] = packet[i+20];
        }
    }

    fprintf(fp,"\nTransport Layer Analysis of the packet\n");
    fprintf(fp,"\t:TCP Header\n");
    fprintf(fp,"Source Port : %d\n", src_port);
    fprintf(fp,"Destination Port : %d\n", dest_port);
    fprintf(fp,"Sequence Number(raw): %d\n", sequence_num);
    fprintf(fp,"Acknowledgement Number(raw): %d\n", ack_num);
    fprintf(fp,"Header Length: %d\n", header_length);
    fprintf(fp,"Flags : 0x%.4X\n", flags);
    fprintf(fp,"Window Size (raw): %d\n", window_size);
    fprintf(fp,"Header Checksum : 0x%.4X\n",checksum);
    for(int i=0;i<header_length-20;i++){
        if(i==0) fprintf(fp,"Options(Byte by byte) : 0x%.2X",options[i]);
        else if (i==header_length-21) fprintf(fp," : %.2X\n",options[i]);
        else fprintf(fp," : %.2X",options[i]);
    }

    struct tcpReturn ret;
    ret.src_port = src_port;
    ret.dest_port = dest_port;
    ret.header_length = header_length;
    return ret;
}
