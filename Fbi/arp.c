#include "imports.h"


void arp(FILE * fp, unsigned char * packet) {
    	    		
	        fprintf(fp,"\nNetwork Layer Protocol Analysis of the above Packet\n");
		fprintf(fp,"\t:Arp Header\n");

		
		fprintf(fp,"Hardware type: 0x");
		for (int i = 0; i < 2; i++) {
        		fprintf(fp,"%02x", packet[i]);
    		}
    		if(ntohs(*(uint16_t *)(packet + 0)) == 0x0001)
    			fprintf(fp,"    -Ethernet");   		
    		fprintf(fp,"\n");

    		
		fprintf(fp,"Protocol type: 0x");
		for (int i = 2; i < 4; i++) {
        		fprintf(fp,"%02x", packet[i]);
    		}
    		if(ntohs(*(uint16_t *)(packet + 2)) == 0x0800)
    			fprintf(fp,"    -ipv4");   		
    		fprintf(fp,"\n");			


		fprintf(fp,"Mac Length: ");
		for (int i = 4; i < 5; i++) {
        		fprintf(fp,"%02d", packet[i]);
    		} 		
    		fprintf(fp,"\n");
    		

		fprintf(fp,"Ip Length: ");
		for (int i = 5; i < 6; i++) {
        		fprintf(fp,"%02d", packet[i]);
    		} 		
    		fprintf(fp,"\n");
    		
    				
		fprintf(fp,"ARP opcode : 0x");
		for (int i = 6; i < 8; i++) {
        		fprintf(fp,"%02x", packet[i]);
    		}
    		if(ntohs(*(uint16_t *)(packet + 6)) == 0x0001)
    			fprintf(fp,"    -Arp Request");
    		else if(ntohs(*(uint16_t *)(packet + 6)) == 0x0002)
    			fprintf(fp,"    -Arp Reply");
    		fprintf(fp,"\n");
    		
    						
    	    	fprintf(fp,"Src mac : ");
		for (int i = 8; i < 14; i++) {
        		fprintf(fp,"%02x ", packet[i]);
    		}
    		fprintf(fp,"\n");

    		
    		fprintf(fp,"Src Ip : ");
		for (int i = 14; i < 18; i++) {
        		fprintf(fp,"%02d.", packet[i]);
    		}
    		fprintf(fp,"\n");
    		
    		
    		fprintf(fp,"destn mac : ");
		for (int i = 18; i < 24; i++) {
        		fprintf(fp,"%02x ", packet[i]);
    		}
    		fprintf(fp,"\n");
    		
    		
    		fprintf(fp,"destn Ip : ");
		for (int i = 24; i < 28; i++) {
        		fprintf(fp,"%02d.", packet[i]);
    		}     				
		fprintf(fp,"\n\n");
     
	
}



