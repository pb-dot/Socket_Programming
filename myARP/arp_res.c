// Accept only those packets whose packet start 6B = own Mac addr
/*  
Setup:
At machine(PB) ip=10.2.16.153  and Mac = 40.b0.34.38.f9.51 
we run "arping 10.2.83.22" which returns mac of 10.2.83.22 = a0.8c.fd.bf.d0.7a

ARP Response Packet format :- 60B (42 B without padding) received by Machine(PB)

<----Ethernet header--6B+6B+2B--=14B-----><---ARP packet---2B+2B+1B+1B+2B+6B+4B+6B+4B----=28B------><---Ethernet Padding optional--18B>

Ethernet header:
1. Target Mac = 40 b0 34 38 f9 51 (PB mac addrr)	:6B
2. Sender Mac = (mac of any device--ignore )		:6B
3. Protocol Ttpe = 08 06  (ARP)				:2B

ARP packet contains:
1.Hardware type = Ethernet (0001h) 			:2B
2.Protocol type = IPv4 (0800h)	   			:2B
3.Mac length = 06h		   			:1B
4.ip legth = 04h		   			:1B
5.arp opcode = response(0002h)	   			:2B
6.sender mac = a0.8c.fd.bf.d0.7a (ANSWER)	  	:6B
7.sender ip =  ans  ip 10.2.83.22 =(0a 02 53 16)	:4B
8.target mac = 	40 b0 34 38 f9 51 (PB mac addrr)  	:6B
9.target ip=  PB ip 10.2.16.153 = (0a 02 10 99)		:4B

Packet:->

<Destn Mac > <Own Mac> <Type ARP=0806h>||<0001h><0800h><06h><04h><0002h><SenderMac><SendIp><TargetMac><Target-IP> || <Padding 18B all 0s>
*/

int unpack_arp_res(const unsigned char * ownMac, unsigned char * packet) {
    	    		
	//check if the packet is arp reply or not and dest mac is my mac addr	
	if(packet[12]==0x08 && packet[13]==0x06 && check(packet,ownMac,0)&& ntohs(*(uint16_t *)(packet + 20)) == 0x0002){
	        printf("\nDoing Analysis of the following Received ARP packet\n");
	        print(packet);
		printf("\t:Ethernet Header\n");
		printf("Destn mac : ");	
		for (int i = 0; i < 6; i++) {
        		printf("%02x ", packet[i]);
    		}
    	    	printf("\n");
    	    	printf("Src mac : ");
		for (int i = 6; i < 12; i++) {
        		printf("%02x ", packet[i]);
    		}
    	    	printf("\n");
    	    	    	    	
		printf("\t:ARP header\n");
		printf("ARP opcode : ");
		for (int i = 20; i < 22; i++) {
        		printf("%02x ", packet[i]);
    		}
    		printf("\n");				
    	    	printf("Src mac : ");
		for (int i = 22; i < 28; i++) {
        		printf("%02x ", packet[i]);
    		}
    		printf("\n");
    		printf("Src Ip : ");
		for (int i = 28; i < 32; i++) {
        		printf("%02d.", packet[i]);
    		}
    		printf("\n");
    		printf("destn mac : ");
		for (int i = 32; i < 38; i++) {
        		printf("%02x ", packet[i]);
    		}
    		printf("\n");
    		printf("destn Ip : ");
		for (int i = 38; i < 42; i++) {
        		printf("%02d.", packet[i]);
    		}     				
		printf("\n\n");

	     	printf("\t:For Target IP : ");
		for (int i = 28; i < 32; i++) {
        		printf("%02d.", packet[i]);
    		}
    		printf("\n");
    		printf("\t:Its Mac is : ");
    		for (int i = 22; i < 28; i++) {
        		printf("%02x ", packet[i]);
    		}
    		printf("\n\n\n");
    		packetsReceived++;
    		return 1;	
     }
	return 0;
}



