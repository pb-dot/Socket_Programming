// pass port number of server as command line argument
// how to use -->prithi@HP$./server.out 9999
#include "common.c"

char cbuffer[255];//store client mssg
char sbuffer[255];// store server mssg
int newsockfd,sockfd,status;//stores socket id and status
int *flag;// stores ending of WriteMode
int *flag_e;// stores session end flag
int *line_count;  // shared memory for line count
char *shared_mem; // shared memory for storing lines
struct sockaddr_in c_addr;//stores client address who connects in network format
socklen_t c_addr_len;//length of c_addr
char client_addr[100];//stores client address who connects in presentable format
#define MAX_LINES 50
#define MAX_LINE_LENGTH 255

void terminate(){
			//close
			printf("Closing Socket\n");
			close(newsockfd);
			close(sockfd);
			// Cleanup
			printf("Closing shared Mem\n");
			shm_unlink("/flag");
			shm_unlink("/flagE");
        		shm_unlink("/line_count");
        		shm_unlink("/lines");
			//kill current process		
			exit(0);
}

// Function to be executed when Ctrl+Z is pressed
void WriteMode() {
    printf("\n\nCtrl+Z pressed! Entered Write Mode...\n");
    while(1){
    //taking server input and sending to client
    		bzero(sbuffer,255);
		printf("Type Server's mssg here :$$ ");
		fgets(sbuffer,255,stdin);
		status=write(newsockfd,sbuffer,255);
		check_err(status,"Error in writing on server side");
		
	  	//breaking loop as per servers wish
		if(strncmp("Bye",sbuffer,3)==0)
		{
			printf("Ending Session By server\n");
			*flag_e=1;
			break;

		}
		//get choice
    		bzero(sbuffer,255);
		printf("Type 'yes' to Exit Write Mode else press Enter :$$ ");
		fgets(sbuffer,255,stdin);
		if(strncmp("yes",sbuffer,3)==0) break;
	}
		//printing the messages from client received in the meantime
		if(*line_count !=0){
		printf("\nMessages received while in Writing Mode\n\n");
		        for (int i = 0; i < *line_count; i++) {
		        	printf("Received from Client@%s|%d :$$ %s \n",client_addr,c_addr.sin_port,shared_mem + (i * MAX_LINE_LENGTH));
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
        	   *line_count=0;
        	   while(1){
        	   	if(*flag==1 || *flag_e==1){//finished write mode
        	   		break;
        	   	}
	   		// receive (waiting for client request)
	   		bzero(cbuffer,255);	   		   				
			status=read(newsockfd,cbuffer,255);			
			check_err(status,"Error in reading on server side");
			
			if(*flag==1){// already exitted writing mode no need to store in sm
			    printf("Received Client@%s|%d :$$ %s \n",client_addr,c_addr.sin_port,cbuffer);
			} 
	   		//breaking loop as per clients wish
			if(strncmp("Bye",cbuffer,3)==0)
			{
				printf("Ending session by Client\n");
					//close session flag set
				*flag_e=1;
				break;
			}			
            		if (*line_count < MAX_LINES) {
                		// Copy the input line to shared memory
                		strcpy(shared_mem + (*line_count * MAX_LINE_LENGTH), cbuffer);
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
    	printf("CTRL+C pressed\n");
    	terminate();
    }
}

int main(int argc, char **argv){
	// checking command line args
	if (argc <2){
		printf("Didnt enter server-port number as command line argument\n");
		exit(1);
	}
/////////////////////////////////////////////////////Shared MeMory Setup ///////////////////	
    
    // Create shared memory for Write Mode flag
    int shm_flag = shm_open("/flag", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_flag, sizeof(int));
    flag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_flag, 0);

    // Create shared memory for Write Mode flag
    int shm_flagE = shm_open("/flagE", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_flagE, sizeof(int));
    flag_e = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_flagE, 0);

    // Create shared memory for line count
    int shm_line_count = shm_open("/line_count", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_line_count, sizeof(int));
    line_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_line_count, 0);

    // Create shared memory for storing lines
    int shm_lines = shm_open("/lines", O_CREAT | O_RDWR, 0666);
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
	sockfd= socket(AF_INET,SOCK_STREAM,0);
	check_err(sockfd,"Error in opening socket");
	
	// setting server details
	struct sockaddr_in sa;
	sa.sin_family=AF_INET;//setting address family to IPv4
	sa.sin_port=htons(atoi(argv[1]));
	sa.sin_addr.s_addr=INADDR_ANY;
	
	// binding
	status= bind(sockfd,(struct sockaddr *) &sa, sizeof(sa));
	check_err(status,"Error in binding");
	
	//listening
	status= listen(sockfd,100);
	check_err(status,"Error in listening");

	//optional messages
	printf("\nServer started --- waiting for client ...\n\n");
		
	//accepting (see declaration at top -global variable)
	c_addr_len=sizeof(c_addr);
	newsockfd= accept(sockfd,(struct sockaddr *) & c_addr,&c_addr_len);
	check_err(newsockfd,"Error in accepting client");
	inet_ntop(AF_INET,&c_addr,client_addr,100);//converting the format and store in client_addr
		
	//optional messages
	printf("Connected to Client@%s|%d \n\n",client_addr,c_addr.sin_port);
	printf("Press CTRL+Z to enter write mode\n");
	printf("Press \"Bye\" to end session when :$$ prompt appears\n\n");

///////////////////////////////////////////////////////////////////////////////////////////////
		
	
	   while(1){
	   // receive (waiting for client request)
	   if(*flag_e==1) terminate();	   		   	
		bzero(cbuffer,255);
		status=read(newsockfd,cbuffer,255);
		check_err(status,"Error in reading on server side");
		printf("Received Client@%s|%d :$$ %s \n",client_addr,c_addr.sin_port,cbuffer);
		fflush(stdout);
		fflush(stdin);
	   //breaking loop as per clients wish
		if(strncmp("Bye",cbuffer,3)==0)
		{
			printf("Ending Session By Client\n");
			break;
		}	
	   }
	   	
	//close
terminate();
}













