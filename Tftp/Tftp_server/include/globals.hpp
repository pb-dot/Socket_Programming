#pragma once
#include "std_headers.h"

/////////////////////////////////// Types of TFTP Packets
class RRQ_WRQ_Packet{
public:
    int block_number;
    char* filename;
    char* mode;
} ;

class DATA_Packet{
public:
    int block_number;
    char* data;
    int data_size;
} ;

class ACK_Packet{
public:
    int block_number;
} ;

class ERROR_Packet{
public:
    int block_number;
    int error_code;
    char* error_message;
} ;
////////////////////////////////////////// contents of Worker Queue and Timer Queue

class job{
public:
    bool resend{false};
    char type; // R for read || W for Write || A for Ack || D for data || E for Error
    int block_num{-1}; // 0 for type is R/W
    int mssg_size; // non zero only if type is D
    std::string client_id; // IP + "_" + Port
    std::string fileName;
    std::string message;// valid only if type is D
    std::chrono::time_point<std::chrono::steady_clock> timestamp;

    void display_job(){
        std::cout<<"Resend = "<<resend<<"\n";
        std::cout<<"type = "<<type<<"\n";
        std::cout<<"Block_num = "<<block_num<<"\n";
        std::cout<<"mssg size = "<<mssg_size<<"\n";
        std::cout<<"client _id = "<<client_id<<"\n";
        std::cout<<"fileName = "<<fileName<<"\n";
        std::cout<<"message = "<<message<<"\n";
        //std::cout<<"time interval till now= "<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timestamp)<<"\n";
    }
}; 

class context{
public:
    std::string fileName;
    std::string mode;
    int block_num;
};
///////////////////////////////////////////// DS and variables //////////////////////

// key/client_id = IP + Port | Value = (FileName , Mode , current_block_num)
inline std::unordered_map<std::string , context> client_context; 

inline std::list<job> WorkQ; // std::string is client_id = IP +Port

inline std::list<job> TimerQ;
inline std::unordered_map<std::string,std::list<job>::iterator>TimerM; // key = client_id

inline std::mutex mtx_WorkQ, mtx_Timer, mtx_context;      
inline std::condition_variable cv_work , cv_timer;// Condition variable to notify timer thread

/////////////////////////////// Helper Functions ////////////////////

inline void check_err(int fd,std::string mssg){
	if(fd <0){
		perror(mssg.c_str());
		exit(1);
	}	
}