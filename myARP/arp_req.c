/*  
Setup:
At machine(PB) ip=10.2.16.153  and Mac = 40.b0.34.38.f9.51 
we run "arping 10.2.83.22" which returns mac of 10.2.83.22 = a0.8c.fd.bf.d0.7a

ARP Request Packet format :- 60B (42 B without padding) sent by Machine (PB)

<----Ethernet header--6B+6B+2B--=14B-----><---ARP packet---2B+2B+1B+1B+2B+6B+4B+6B+4B----=28B------>
<---Ethernet Padding optional--18B>

Ethernet header:
1. Target Mac = ff ff ff ff ff ff (broadcast)		:6B
2. Sender Mac = 40 b0 34 38 f9 51 (own mac addrr)	:6B
3. Protocol Ttpe = 08 06  (ARP)				:2B

ARP packet contains:
1.Hardware type = Ethernet (0001h) 			:2B
2.Protocol type = IPv4 (0800h)	   			:2B
3.Mac length = 06h		   			:1B
4.ip legth = 04h		   			:1B
5.arp opcode = request(0001h)	   			:2B
6.sender mac = 40 b0 34 38 f9 51 (own mac addrr)  	:6B
7. own ip =  lets say 10.2.16.153 = (0a 02 10 99) 	:4B
8.target mac = (all 0 or all 1s) [dont care]	  	:6B
9. target ip=  lets say 10.2.83.22 =(0a 02 53 16) 	:4B

Packet:->

<Destn Mac > <Own Mac> <Type ARP=0806h>||<0001h><0800h><06h><04h><0001h><SenderMac><SendIp><TargetMac><Target-IP> || <Padding 18B all 0s>
*/

unsigned char * pack_arp_req(const unsigned char* ownMac,const unsigned char* ownIp,const unsigned char* destnIp ) {
    // Ethernet header
    unsigned char ethernetHeader[14]; // 6 bytes for destination MAC + 6 bytes for source MAC + 2 bytes for protocol type

    // Target MAC (broadcast)
    memset(ethernetHeader, 0xff, 6);

    // Sender MAC
    memcpy(ethernetHeader + 6, ownMac, 6);

    // Protocol type (ARP)
    uint16_t *protocolType = (uint16_t*)(ethernetHeader + 12);
    *protocolType = htons(0x0806);

    // ARP packet
    unsigned char arpPacket[28]; // Total size of ARP packet is 28 bytes

    // Hardware type (Ethernet)
    uint16_t *hardwareType = (uint16_t*)arpPacket;
    *hardwareType = htons(0x0001);

    // Protocol type (IPv4)
    uint16_t *protocolTypeARP = (uint16_t*)(arpPacket + 2);
    *protocolTypeARP = htons(0x0800);

    // MAC length
    arpPacket[4] = 0x06;

    // IP length
    arpPacket[5] = 0x04;

    // ARP opcode (request)
    uint16_t *arpOpcode = (uint16_t*)(arpPacket + 6);
    *arpOpcode = htons(0x0001);

    // Sender MAC (own MAC address)
    memcpy(arpPacket + 8, ownMac, 6);

    // Sender(Own) IP (10.2.19.47)
    struct in_addr senderIp;
    inet_pton(AF_INET, ownIp, &senderIp);
    memcpy(arpPacket + 14, &senderIp.s_addr, 4);

    // Target MAC (don't care)
    memset(arpPacket + 18, 0x00, 6);

    // Target(destn) IP (10.2.100.212)//dipmays ip
    struct in_addr targetIp;
    inet_pton(AF_INET, destnIp, &targetIp);
    memcpy(arpPacket + 24, &targetIp.s_addr, 4);

    // Char array to hold the packet
    unsigned char *packet = malloc(60*sizeof(char));

    // Copy Ethernet header
    memcpy(packet, ethernetHeader, 14);

    // Copy ARP packet
    memcpy(packet + 14, arpPacket, 28);

    // Add 18 bytes of all zeros
    memset(packet + 42, 0x00, 18);

    // Print the packet
    printf("Complete ARP packet generated: ");
    print(packet);

    return packet;
}



