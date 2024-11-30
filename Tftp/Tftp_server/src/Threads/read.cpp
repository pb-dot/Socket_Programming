#include "globals.hpp"
#include "packets.hpp"

void reader_thread(int sockfd){

    //setup
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    u_char buffer[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
    std::string client_key;

    while (true) {
        // read from socket()
        bzero(buffer,BUFFER_SIZE);
        ssize_t received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        //auto recv_timestamp = std::chrono::steady_clock::now();
        check_err(received_bytes,"recvfrom failed");
        buffer[received_bytes] = '\0';  // Null-terminate the received data
        

        // Convert client address to a readable IP and port
        bzero(client_ip,INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        client_port = ntohs(client_addr.sin_port);

        // Create a unique key associated with every client
        client_key = std::string(client_ip) + "_" + std::to_string(client_port);
        
        //debug::
        std::cout<<" Just received a packet from "<<client_key<<" and start decoding his packet\n";

        // decode the received buffer
        switch(buffer[1]){
            case 0x01: // Read Packet
            case 0x02: // Write Packet
            {
                    
                    RRQ_WRQ_Packet rw_packet = extract_rrq_wrq_packet(buffer);
                    
                    // create job for Work Queue
                    job thejob;
                    if(buffer[1] == 0x01) thejob.type = 'R';
                    if(buffer[1] == 0x02) thejob.type = 'W';
                    thejob.block_num = rw_packet.block_number;
                    thejob.mssg_size = 0;
                    thejob.client_id = client_key;
                    thejob.fileName = std::string(rw_packet.filename);
                    thejob.message ="";
                    thejob.timestamp = std::chrono::steady_clock::time_point::min();
                   
                    // update the global data structure client_context                        
                    mtx_context.lock();
                    // if new client detected thus new mssg
                    if(client_context.find(client_key) == client_context.end()){// not found
                        client_context[client_key].fileName = thejob.fileName;
                        client_context[client_key].mode= std::string(rw_packet.mode);
                        client_context[client_key].block_num = thejob.block_num;

                        mtx_context.unlock();
                        // update the global data structure WorkQueue
                        {
                            std::lock_guard<std::mutex> lock2(mtx_WorkQ);
                            WorkQ.push_back(thejob);
                        }

                        //debug:
                        std::cout<<"Inside Reader Thread Added R/W job to workQ\n";
                    }
                    else{
                        mtx_context.unlock(); 
                        // context for client_key exists and recv is RD/WR 
                        //=> duplicate mssg from client recived
                    }
            
                    //std::cout<<"Inside Read Thread : R/W case  release lock\n";
                    // cleanup:-
                    free(rw_packet.filename);
                    free(rw_packet.mode);

            }
                    break;
            case 0x03:
            {
                    DATA_Packet  data_packet  = extract_data_packet(buffer, received_bytes);

                    // create job for Work Queue;
                    job thejob;
                    thejob.type ='D';
                    thejob.block_num = data_packet.block_number;
                    thejob.client_id = client_key;                   
                    thejob.message = std::string(data_packet.data);
                    thejob.mssg_size = data_packet.data_size;
                    thejob.timestamp = std::chrono::steady_clock::time_point::min();
                
                    // the global data structure:- access client_context | update TimerQ WorkQ                                      
                    mtx_context.lock();
                    thejob.fileName = client_context[client_key].fileName;

                    //recv block_num > present block_num  ==> new block recv
                    if(thejob.block_num > client_context[client_key].block_num){
                        
                        //update  client_Context
                        client_context[client_key].block_num = thejob.block_num;
                        mtx_context.unlock();

                        {// update Work Q
                        std::lock_guard<std::mutex> lock2(mtx_WorkQ);
                        WorkQ.push_back(thejob);}

                        //debug:
                        std::cout<<"Inside Reader Thread Added D job to workQ\n";

                        {// [Remove entry from timer if new mssg recev before timer expire]
                        std::lock_guard<std::mutex> lock3(mtx_Timer);
                        if(TimerM.find(client_key)!= TimerM.end()){
                            //if(std::chrono::duration_cast<std::chrono::milliseconds>(recv_timestamp-TimerM[client_key]->timestamp) < threshold ){
                                TimerQ.erase(TimerM[client_key]);
                                TimerM.erase(client_key);
                            //}
                        } }

                    }else{
                        mtx_context.unlock();
                        // The message received is a repeated one : do nothing
                    }
                         
                    //std::cout<<"Inside Read Thread : D case release lock\n";
                    //cleanup:
                    free(data_packet.data);


            }
                    break;
            case 0x04:
            {
                    ACK_Packet the_ack_packet = extract_ack_packet(buffer);
                    
                    // create job for Work Queue;
                    job thejob;
                    thejob.type ='A';
                    thejob.block_num = the_ack_packet.block_number;
                    thejob.client_id = client_key;
                    thejob.message = "";
                    thejob.mssg_size = 0;
                    thejob.timestamp = std::chrono::steady_clock::time_point::min();
                    
                    // the global data structure:- access client_context | update TimerQ WorkQ 

                    mtx_context.lock();
                    thejob.fileName = client_context[client_key].fileName;

                    //recv block_num > present block_num  ==> new block recv
                    if(thejob.block_num > client_context[client_key].block_num){
                        
                        //update  client_Context
                        client_context[client_key].block_num = thejob.block_num;
                        mtx_context.unlock();

                        {// update Work Q
                        std::lock_guard<std::mutex> lock2(mtx_WorkQ);
                        WorkQ.push_back(thejob);}

                        //debug:
                        std::cout<<"Inside Reader Thread Added A job to workQ\n";

                        {// [Remove entry from timer if new mssg recev before timer expire]
                        std::lock_guard<std::mutex> lock3(mtx_Timer);
                        if(TimerM.find(client_key)!= TimerM.end()){
                            //if(std::chrono::duration_cast<std::chrono::milliseconds>(recv_timestamp-TimerM[client_key]->timestamp) < threshold ){
                                TimerQ.erase(TimerM[client_key]);
                                TimerM.erase(client_key);
                            //}
                        }}

                    }else{
                        mtx_context.unlock();
                        // The message received is a repeated one : do nothing
                    }
                    
            }
                    break;
            case 0x05:
                    std::cout<<"Received error message from client\n";
                    //ERROR_Packet err = extract_error_packet(buffer,received_bytes);
                    break;
            default :
                    std::cout<<" Received Wrong Packet Opcode\n";
                    exit(0);
        }
        
        cv_work.notify_one(); // notifies the frwd thread that WorQ has data

    }



}