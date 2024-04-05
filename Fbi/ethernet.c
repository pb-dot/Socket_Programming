#include "imports.h"


uint16_t ethernet(FILE * fp, unsigned char * packet) {
    	    		
	        fprintf(fp,"\nData Link Layer Protocol Analysis of the above Packet\n");
		fprintf(fp,"\t:Ethernet Header\n");
		fprintf(fp,"Destn mac : ");	
		for (int i = 0; i < 6; i++) {
        		fprintf(fp,"%02x ", packet[i]);
    		}
    	    	fprintf(fp,"\n");
    	    	fprintf(fp,"Src mac : ");
		for (int i = 6; i < 12; i++) {
        		fprintf(fp,"%02x ", packet[i]);
    		}
    	    	fprintf(fp,"\n");
    	    	fprintf(fp,"Next Layer Protocol Opcode : 0x");
		for (int i = 12; i < 14; i++) {
        		fprintf(fp,"%02x", packet[i]);
    		}
    	    	fprintf(fp,"\n");
     
	return ntohs(*(uint16_t *)(packet + 12));
}



