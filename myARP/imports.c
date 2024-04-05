#include <stdio.h>  //for printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <signal.h>
#include <errno.h> //For errno - the error number
#include <unistd.h> // sleep()
#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>	//for socket ofcourse
#include <sys/ioctl.h>

#include <netinet/tcp.h> //Provides declarations for tcp header
#include <netinet/udp.h> //Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header


#include <net/if.h>
#include <net/ethernet.h> /* the L2 protocols */

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>


#include <arpa/inet.h> // inet_addr


//signal handler for ctrl+c
void sigint_handler(int signum) {
    printf("\nCtrl+C received. Pausing...Enter any char to restart\n");
    getchar();
}    

/*// Function to convert a string representation of a MAC address to binary
void macStringToBinary(const unsigned char *macString, unsigned char *macBinary) {
    sscanf(macString, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &macBinary[0], &macBinary[1], &macBinary[2],
           &macBinary[3], &macBinary[4], &macBinary[5]);
           
}
*/
void print(unsigned char *packet){
    	
    	    for (int i = 0; i < 60; i++) {
        	printf("%02x ", packet[i]);
    		}
    	    printf("\n");

}

int check(unsigned char* packet,const unsigned char *mac,int start){

    for(int i=start;i<start+6;i++){
	if((uint8_t)packet[i]!=(uint8_t)mac[i-start])
		return 0;
		    
    }
	return 1;
}



