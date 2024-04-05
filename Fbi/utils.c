#include "imports.h"
#include "utility_header.h"
/////////////////////////////// Below Code is For Command Line Parsing ///////////////////////


// Function to print usage instructions
void printUsage() {
    printf("Correct Usage Format: ./fbi -i interface_Name [-n number_Of_packets] [-p protocol_name]\n");
}

// Function to parse command line arguments
struct Arguments parseArguments(int argc, char *argv[]) {

	if(argc <= 1){
		printf("Error: Invalid Command Line Arguments\n");
		printUsage();
		exit(EXIT_FAILURE);
	}

    struct Arguments args;
    args.interfaceName = NULL;
    args.numberOfPackets = -1;
    args.protocolName = NULL;

    // Iterate through command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            // -i flag found, next argument should be interface name
            if (i + 1 < argc) {
                args.interfaceName = argv[i + 1];
                i++; // Skip next argument
            } else {
                printf("Error: Missing interface name after -i flag.\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            // -n flag found, next argument should be number of packets
            if (i + 1 < argc) {
                args.numberOfPackets = atoi(argv[i + 1]);
                i++; // Skip next argument
            } else {
                printf("Error: Missing number of packets after -n flag.\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            // -p flag found, next argument should be protocol name
            if (i + 1 < argc) {
                args.protocolName = argv[i + 1];
                i++; // Skip next argument
            } else {
                printf("Error: Missing protocol name after -p flag.\n");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Error: Unknown flag or argument '%s'\n", argv[i]);
            printUsage();
            exit(EXIT_FAILURE);
        }
    }

    // Check if interface name is provided
    if (args.interfaceName == NULL) {
        printf("Error: Interface name is required.\n");
        printUsage();
        exit(EXIT_FAILURE);
    }

    return args;
}


///////////////////////////// Below Code to be used in main.c///////////////////////

int createRawSocket(const char* interfaceName,const char* protocolName) {
    int sock_recv;

    if (protocolName == NULL) {
        // Protocol name is NULL, create a raw socket for all protocols
        sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    } else if (strcmp(protocolName, "arp") == 0) {
        // Protocol name is ARP
        sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    } else if (strcmp(protocolName, "ip") == 0) {
        // Protocol name is IP
        sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    } else if (strcmp(protocolName, "ipv6") == 0) {
        // Protocol name is IPv6
        sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IPV6));
    } else {
        // Unsupported protocol receive all
        sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    }

    if (sock_recv == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set promiscuous mode
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interfaceName, IFNAMSIZ);
    if (ioctl(sock_recv, SIOCGIFFLAGS, &ifr) == -1) {
        perror("ioctl failed");
        exit(EXIT_FAILURE);
    }
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(sock_recv, SIOCSIFFLAGS, &ifr) == -1) {
        perror("ioctl failed");
        exit(EXIT_FAILURE);
    }
    return sock_recv;
}


void getMacIp(int sock_recv,char * interfaceName,unsigned char *Ip,unsigned char *Mac){
    struct sockaddr* ip;struct sockaddr* mac;
    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strncpy(ifr.ifr_name,interfaceName,IFNAMSIZ-1);

    if((ioctl(sock_recv,SIOCGIFINDEX,&ifr))<0)
        printf("error in index ioctl reading");//getting Index Name
    
    int ifIndex = ifr.ifr_ifindex;

    ioctl(sock_recv,SIOCGIFNAME,&ifr);
    printf("\nInterface name is : %s and id is %d\n",ifr.ifr_name,ifIndex);

    
    if(ioctl(sock_recv,SIOCGIFADDR,&ifr)<0)
    	printf("error in Ip ioctl reading");// getting Ip address
    	
    ip = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    memcpy(ip,&ifr.ifr_addr,sizeof(struct sockaddr));
    printf("Your IP is : %s\n",inet_ntop(ip->sa_family,&(((struct sockaddr_in*)ip)->sin_addr),Ip,INET6_ADDRSTRLEN));
        
    if(ioctl(sock_recv,SIOCGIFHWADDR,&ifr)<0)
    	printf("Error in Mac ioctl reading"); // getting Mac address
    mac = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    memcpy(mac,&ifr.ifr_hwaddr,sizeof(struct sockaddr));
    printf("Your MAC Address is : ");
    for(int i=0;i<6;i++){
        Mac[i]=(unsigned char)mac->sa_data[i];
        printf("%02X",(unsigned char)Mac[i]);
        if(i!=5) printf(":");
    }
    printf("\n\n\n");
}


void writeHeaderToFile(const char *filename, const char *text,unsigned char *Ip,unsigned char * Mac,char * interfaceName) {
    FILE *file = fopen(filename, "a"); // Open file in append mode
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(file,"\n########################################################################\n\n");
    fprintf(file, "%s\n", text); // Write text to file
    fprintf(file, "Your network interface is : %s\n",interfaceName);
    fprintf(file, "Your Ip is : %s\n",Ip);
    fprintf(file, "Your Mac is : ");
    for(int i=0;i<6;i++){
        fprintf(file,"%02x",(unsigned char)Mac[i]);
        if(i!=5) fprintf(file,":");
    }
    
    fprintf(file,"\n\nA packet can be broken down into the following segements\n");
    fprintf(file,"\n-------------------------------------------\n");
    fprintf(file,"| Layer-4 | Layer -3 | Layer-2 | Layer -1 |\n");
    fprintf(file,"-------------------------------------------\n\n");
    
    fprintf(file,"Layer-4 a.k.a Data-Link-Layer: Supported Protocol - Ethernet\n");
    fprintf(file,"Layer-3 a.k.a Network-Layer: Supported Protocol - ARP | Ipv4 | Ipv6 | Icmp\n");
    fprintf(file,"Layer-2 a.k.a Transport-Layer: Supported Protocol - TCP | UDP \n");
    fprintf(file,"Layer-1 a.k.a Application-Layer: Supported Protocol - DNS | *\n");
    fprintf(file,"\n########################################################################\n\n");
    fclose(file); // Close file
}

void printPacket(FILE * fp,unsigned char *packet, int start, int end){
   	       	     	
    	    for (int i = start; i < end; i++) {
        	fprintf(fp,"%02x ", packet[i]);
    		}
    	    fprintf(fp,"\n");

}

int checkMac(unsigned char* packet,const unsigned char *mac,int start){

    for(int i=start;i<start+6;i++){
	if((uint8_t)packet[i]!=(uint8_t)mac[i-start])
		return 0;
		    
    }
	return 1;
}



