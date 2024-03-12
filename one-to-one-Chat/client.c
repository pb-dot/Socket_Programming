// pass port number of server as command line argument
// how to use -->prithi@HP$./client.out 127.0.0.1 9999
#include "common.c"

char cbuffer[255];//store client mssg
char sbuffer[255];// store server mssg
char server_ip[255];//stores server ip
char server_port[10];//stores server port
int  newsockfd,status;//stores socket id and status
int *flag;// stores ending of WriteMode
int *flag_e;// stores session end flag
int *line_count;  // shared memory for line count
char *shared_mem; // shared memory for storing lines

#define MAX_LINES 50
#define MAX_LINE_LENGTH 255

void terminate(){
			//close
			printf("Closing Socket\n");
			close(newsockfd);
			// Cleanup
			printf("Closing shared Mem\n");
			shm_unlink("/flag1");
			shm_unlink("/flagE1");
        		shm_unlink("/line_count1");
        		shm_unlink("/lines1");
			//kill current process		
			exit(0);
}

// Function to be executed when Ctrl+Z is pressed
void WriteMode() {
	
    printf("\n\nCtrl+Z pressed! Entered Write Mode...\n");
    while(1){
    //Taking client input and sending to server
    		bzero(cbuffer,255);
		printf("Type Clients's mssg here :$$ ");
		fgets(cbuffer,255,stdin);
		status=write(newsockfd,cbuffer,255);
		check_err(status,"Error in writing on client side");
	  	//breaking loop as per clients wish
		if(strncmp("Bye",cbuffer,3)==0)
		{
			printf("Session Ended by client side\n");
			*flag_e=1;
			break;

		}
		
    		bzero(cbuffer,255);
		printf("Type 'yes' to Exit Write Mode else press Enter :$$ ");
		fgets(cbuffer,255,stdin);
		if(strncmp("yes",cbuffer,3)==0) break;		
	}
		//printing the messages from server received in the meantime
		if(*line_count !=0){
		
			printf("\nMessages received while in Writing Mode\n\n");
		        for (int i = 0; i < *line_count; i++) {
		        	printf("Received from Server@%s|%s :$$ %s \n",server_ip,server_port,shared_mem + (i * MAX_LINE_LENGTH));

        		}
        	}	
		
		*flag=1;//marking the end of WriteMOde
		printf("Exiting Write Mode\n\n");
		return;
			
}

// Signal handler function
void signalHandler(int signo) {

    if (signo == SIGTSTP) {
        pid_t pid = fork();
        if(pid!=0){//parent
        	*flag=0;
        	WriteMode();        	
		if(*flag_e==1) terminate();		
        }
        else{//child
        //while child is writing continue to receive message from server
        	   *line_count=0;
        	   while(1){
        	   	if(*flag==1 || *flag_e==1){//finished write mode
        	   		break;
        	   	}
			// receive response from server					
			bzero(sbuffer,255);			
			status=read(newsockfd,sbuffer,255);
			check_err(status,"Error in reading on client side");
			if(*flag==1){// already exitted writing mode no need to store in sm
				printf("\nReceived Server@%s|%s :$$ %s \n",server_ip,server_port,sbuffer);				
			}            		
	   		//breaking loop as per servers wish
			if(strncmp("Bye",sbuffer,3)==0)
			{
				printf("Session Ended by Server\n");
				//close
				*flag_e=1;
				break;
			}
			// storing in shm
            		if (*line_count < MAX_LINES) {
                		// Copy the received servers mssg to shared memory
                		strcpy(shared_mem + (*line_count * MAX_LINE_LENGTH), sbuffer);
                		(*line_count)++;
            		} else {
                		perror("Shared memory is full. Parent process needs to read lines.\n");
                		exit(0);
            		}			
	   	   }
	   	if(*flag_e==1) terminate();
		exit(0);
	   	      	      
        }
    }
    if(signo == SIGINT){
    	terminate();
    }
    
}

int main(int argc, char **argv){
	// checking command line args
	if (argc <3){
		printf("Didnt enter server-port/ip/both as command line argument\n");
		exit(1);
	}
	strcpy(server_ip,argv[1]);
	strcpy(server_port,argv[2]);
/////////////////////////////////////////////////////Shared MeMory Setup ///////////////////	
    
    // Create shared memory for write mode flag
    int shm_flag = shm_open("/flag1", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_flag, sizeof(int));
    flag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_flag, 0);
    
    // Create shared memory for session end flag
    int shm_flagE = shm_open("/flagE1", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_flagE, sizeof(int));
    flag_e = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_flagE, 0);
    
    // Create shared memory for line count
    int shm_line_count = shm_open("/line_count1", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_line_count, sizeof(int));
    line_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_line_count, 0);

    // Create shared memory for storing lines
    int shm_lines = shm_open("/lines1", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_lines, MAX_LINES * MAX_LINE_LENGTH);
    shared_mem = mmap(NULL, MAX_LINES * MAX_LINE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_lines, 0);

    *line_count = 0;
    *flag=0;
    *flag_e=0;
///////////////////////////////////////////////////////////////////////////////////////////
    	// Set up the signal handler
    	signal(SIGTSTP, signalHandler);
    	signal(SIGINT, signalHandler);
////////////////////////////////////////////////////////////////////////////////////////////	
	//creating the socket
	newsockfd= socket(AF_INET,SOCK_STREAM,0);
	check_err(newsockfd,"Error in opening socket");
	
	// setting server details
	struct sockaddr_in sa;
	sa.sin_family=AF_INET;//setting address family to IPv4
	sa.sin_port=htons(atoi(argv[2]));
	status = inet_pton(AF_INET,argv[1],&sa.sin_addr);//converting from presentable 
	//format to networkformat and storing in sa.sin_addr
	check_err(status,"Error in client ip conversion format");
	
	// connecting
	status= connect(newsockfd,(struct sockaddr *) &sa, sizeof(sa));
	check_err(status,"Error in connecting");
	
	//optional message
	printf("\nConnected to Server@%s|%s  ...\n\n",argv[1],argv[2]);
	printf("Press CTRL+Z to Enter Write Mode\n");
	printf("Type \"Bye\" to close session when :$$ prompt appears\n\n");


///////////////////////////////////////////////////////////////////////////////////////////////
		
	//continuosly receive message from server
		while(1){	
		// receive response from server	
		if(*flag_e==1) terminate();		
			bzero(sbuffer,255);			
			status=read(newsockfd,sbuffer,255);
			check_err(status,"Error in reading on client side");
			printf("\nReceived Server@%s|%s :$$ %s \n",argv[1],argv[2],sbuffer);
	
		//breaking loop as per servers wish
			if(strncmp("Bye",sbuffer,3)==0)
			{
				printf("Session Ended By Server\n");
				break;
			}						 
		}
	   	
terminate();
}













