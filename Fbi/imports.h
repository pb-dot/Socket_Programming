
///////////////////////////// libc dependency ////////////////

#include <stdio.h>  //for printf
#include <stdint.h> // for uint16_t
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <errno.h> //For errno - the error number
#include <unistd.h> // sleep()
#include <time.h>


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
