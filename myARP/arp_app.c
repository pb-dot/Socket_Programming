#include "imports.c"

// how to run ./app.out <iterfaceName> <targetIp>
//target Ip is the Ip of the machine whose Mac u want

/*
How to check own system mac(ether) and ip(inet)
prithi@HP-250-G6:~$ ifconfig
eno1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.2.19.47  netmask 255.255.0.0  broadcast 10.2.255.255
        inet6 fe80::6452:1f17:27ad:33df  prefixlen 64  scopeid 0x20<link>
        ether 18:60:24:c1:08:89  txqueuelen 1000  (Ethernet)
*/
//Intially semaphore send =1 reply=time =0
/*
Send Thread		Recv Thread 			Time Thread
P(send)			P(reply)			P(time)
//send()		//flag_arp_res=recv()		//check timeout
			if(flag_arp_res) V(send)	if(timeout) V(send)
V(reply)		else    V(time)			else	 V(reply)

*/


#define IFHWADDRLEN 6

struct sockaddr* ip;struct sockaddr* mac;
unsigned char Ip[INET6_ADDRSTRLEN];
unsigned char Mac[6];
int sock_send,sock_recv,ifIndex;
pthread_t arpSender, arpReciever;
int packetsSent=0, packetsReceived=0;

void *sender(void *args);
void *receiver(void *args);
void terminate(int signum);


#define TIMEOUT_DURATION 5 // Timeout duration in seconds

sem_t mutex; // Semaphore for mutual exclusion
sem_t sent_signal; // Semaphore for signaling packet sent
sem_t received_signal; // Semaphore for signaling packet received
sem_t sender_allowed; // Semaphore to control sender's ability to send packets

time_t last_sent_time; // Shared variable to store last sent time



#include "arp_req.c"
#include "arp_res.c"



void *sender(void *args){

    // Prepare destination address
 
struct sockaddr_ll sadr_ll;
sadr_ll.sll_ifindex = ifIndex; // index of interface
sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
sadr_ll.sll_addr[0] = 0xff;
sadr_ll.sll_addr[1] = 0xff;
sadr_ll.sll_addr[2] = 0xff;
sadr_ll.sll_addr[3] = 0xff;
sadr_ll.sll_addr[4] = 0xff;
sadr_ll.sll_addr[5] = 0xff;
	
	const unsigned char * sendbuff = pack_arp_req(Mac,Ip,args);//present in arp_req.c file
	int send_len;
	
	while(1){
	         // Wait until allowed to send
        	sem_wait(&sender_allowed);
        	
	    // Send packet
	    packetsSent++;
		send_len = sendto(sock_send,sendbuff,60,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
		if(send_len<0)
		{
			perror("Cant send packet");
			close(sock_send);
			exit(-1);
		 
		}
		if(packetsSent == 1) {
		    printf("ARP Packet sent successfully!\n");
		}
		
		// Critical section
		sem_wait(&mutex);
		last_sent_time = time(NULL);
		sem_post(&mutex);
		
		
		sem_post(&sent_signal); // Signal packet sent that means start
		sleep(2);
		
	}

    pthread_exit(0);

}

void *receiver(void *args){

    unsigned char* response = (unsigned char*) malloc(60);//buffer to store response
    bzero(response,60);
    int received;
    // Prepare destination address
    struct sockaddr src_addr;
    socklen_t saddr_len = sizeof(src_addr);
    
    while(1)
    {	
    
    // Wait for packet sent signal
        sem_wait(&sent_signal); // untill a packet is send cant proceed 
        
    	bzero(response,60);
    	
    	saddr_len = sizeof(src_addr);
    	if (recvfrom(sock_recv, response, (size_t)60, 0, &src_addr, &saddr_len) < 0){
	perror("packet send failed");
	exit(EXIT_FAILURE);
	}
		
	received = unpack_arp_res(Mac,response);//present in arp_res.c file
	
	if (received) {
            sem_post(&sender_allowed); // Signal to send another packet
        }
        else{
            sem_post(&received_signal); // Signal to check Timer
        } 
    }

    pthread_exit(0);

}

void terminate(int signum){
    close(sock_send);
    close(sock_recv);

    printf("\n-------------------------------------------------\n");
    printf("Packets Sent : %d\tPackets Recieved : %d\n",packetsSent,packetsReceived);
    printf("Packets Lost : %d\tPercentage Loss: %.4lf\n",packetsSent-packetsReceived,(packetsSent-packetsReceived)*100.0/packetsSent);
    printf("-------------------------------------------------\n");
    exit(0);
}


//argv[1] =iterface name argv[2]= target Ip
int main(int argc, char **argv){

    if(argc<2){
        fprintf(stderr,"Not Enough Arguments\n");
        return -1;
    }
    
    //intialising semaphore
    sem_init(&mutex, 0, 1);
    sem_init(&sent_signal, 0, 0);
    sem_init(&received_signal, 0, 0);
    sem_init(&sender_allowed, 0, 1); // Initialize to allow sender to send
        
    signal(SIGINT,terminate); 

    
    sock_send = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    sock_recv = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(!sock_send || !sock_recv){
        perror("socket() failed ");
        exit(1);
    }
    
    struct ifreq ifr;
    bzero(&ifr,sizeof(struct ifreq));
    strncpy(ifr.ifr_name,argv[1],IFNAMSIZ-1);

    if((ioctl(sock_send,SIOCGIFINDEX,&ifr))<0)
        printf("error in index ioctl reading");//getting Index Name
    
    ifIndex = ifr.ifr_ifindex;

    ioctl(sock_send,SIOCGIFNAME,&ifr);
    printf("\nInterface name is : %s\n",ifr.ifr_name);

    
    if(ioctl(sock_send,SIOCGIFADDR,&ifr)<0)
    	printf("error in Ip ioctl reading");// getting Ip address
    	
    ip = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    memcpy(ip,&ifr.ifr_addr,sizeof(struct sockaddr));
    printf("My IP is : %s\n",inet_ntop(ip->sa_family,&(((struct sockaddr_in*)ip)->sin_addr),Ip,sizeof(Ip)));
        
    if(ioctl(sock_send,SIOCGIFHWADDR,&ifr)<0)
    	printf("Error in Mac ioctl reading"); // getting Mac address
    mac = (struct sockaddr *)malloc(sizeof(struct sockaddr));
    memcpy(mac,&ifr.ifr_hwaddr,sizeof(struct sockaddr));
    printf("My MAC Address is : ");
    for(int i=0;i<6;i++){
        Mac[i]=(unsigned char)mac->sa_data[i];
        printf("%02X",(unsigned char)Mac[i]);
        if(i!=5) printf(":");
    }
    printf("\n\n\n");

    
    pthread_create(&arpSender,NULL,sender,argv[2]);
    pthread_create(&arpReciever,NULL,receiver,NULL);
    
    	
	// Main thread Implementing Timer	
    while (1) {
        // Wait for packet received signal or timeout
        sem_wait(&received_signal);
        
        // Check if packet is received within timeout duration
        sem_wait(&mutex);
        time_t current_time = time(NULL);
        if ((current_time - last_sent_time) > TIMEOUT_DURATION) {
            printf("Timeout: Packet not received within timeout duration\n");
            sem_post(&sender_allowed); // Allow sender to send a new packet
        }
        else
        	sem_post(&sent_signal);
        sem_post(&mutex);
    }
    
    
    pthread_join(arpSender,NULL);
    pthread_join(arpReciever,NULL);

    return 0;

return 0;
}
