#include "globals.hpp"
#include "packets.hpp"

std::pair<unsigned char*,size_t> readFileBlock(const std::string& fileName, int block_num, const std::string& mode) {
    // Determine the file opening mode based on the input "mode"
    const char* fileMode = (mode == "octet") ? "rb" : "r";  // "rb" for binary, "r" for text

    // Open the file in the specified mode
    FILE* file = fopen(fileName.c_str(), fileMode);
    if (!file) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return {nullptr,0};
    }

    //std::cout<<"File size is "<<ftell(file) << " fileMode "<<fileMode<<" fileName = "<< fileName.c_str()<<"\n";
    
    // Calculate the offset and seek to that position in the file
    long offset = block_num * 512;
    if (fseek(file, offset, SEEK_SET) != 0) {
        std::cerr << "Error seeking in file." << std::endl;
        fclose(file);
        return {nullptr,0};
    }

    // Check if the file pointer is beyond the end of the file
    long currentPos = ftell(file);
    fseek(file, 0, SEEK_END);  // Go to the end of the file to find its length
    long fileSize = ftell(file);

    if (currentPos >= fileSize) {
        std::cerr<<"currentPos = "<<currentPos<<" file Size = "<<fileSize<<"\n";
        std::cerr << "Attempted to read beyond the end of file." << std::endl;
        std::cerr << "fileName = "<<fileName <<" block_num = "<<block_num<<" mode = "<<mode<<std::endl;
        fclose(file);
        return {nullptr,0};
    }

    // Go back to the original position after checking file size
    fseek(file, currentPos, SEEK_SET);
    
    // Allocate memory for reading 512 bytes
    unsigned char* buffer = new unsigned char[512];
    std::memset(buffer, 0, 512);  // Initialize buffer with 0

    // Read 512 bytes into buffer
    size_t bytesRead = fread(buffer, sizeof(unsigned char), 512, file);
    if (bytesRead != 512) {
        std::cerr << "Warning: Could not read full 512 bytes, read " << bytesRead << " bytes." << std::endl;
    }

    // Close the file
    fclose(file);

    return {buffer,bytesRead};
}

void writeFileBlock(const std::string& fileName, int block_num, const std::string& mode, std::string data , int data_size){
    // Determine the file opening mode based on the input "mode"
    const char* fileMode = (mode == "octet") ? "ab" : "a";  // "ab" for binary, "a" for text

    // Open the file in the specified mode
    FILE* file = fopen(fileName.c_str(), fileMode);
    if (!file) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    //writing to file
    fwrite(data.c_str(), sizeof(unsigned char), data_size, file);

    // Close the file
    fclose(file);

}

void frwd_thread(int sockfd){
    //setup
    job thejob;
    sockaddr_in client_addr;

    while(true){
        
        // Remove front job from the Work Q
        {
            std::unique_lock<std::mutex> lock2(mtx_WorkQ);
            cv_work.wait(lock2, [] { return !WorkQ.empty(); });

            thejob = WorkQ.front();
            WorkQ.pop_front();
        }

        //debug lines
        std::cout<<"Inside Frwd Thread After Extract Job\n";
        thejob.display_job();

        // Parse the IP and port from the client key
        size_t pos = thejob.client_id.find('_');
        std::string client_ip = thejob.client_id.substr(0, pos);
        int client_port = std::stoi(thejob.client_id.substr(pos + 1));

        // Set up the client address structure     
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        inet_pton(AF_INET, client_ip.c_str(), &client_addr.sin_addr);
        client_addr.sin_port = htons(client_port);

        //process the job
        if(thejob.resend == true){ // this job is pushed from timerQ to WorkQ
            //I just resend the message as it is

            // Send the message to the client
            ssize_t sent_bytes = sendto(sockfd, thejob.message.c_str(), thejob.mssg_size, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
            auto sent_timestamp = std::chrono::steady_clock::now();
            check_err(sent_bytes,"sendto failed");

            //debug:
            std::cout<<"Inside Frwd Thread Just Resent the packet\n";

            //building the job for timer thread
            thejob.timestamp = sent_timestamp;                        

        }
        else{ // this job is from the read thread [I have to built packet based on job type]

            switch(thejob.type){
                case 'R': // recv R req from client ==> send 0th offset data block(blk_num =1) to client
                {                    
                    // prepare packet to be sent
                    std::string mode;
                    {
                        std::lock_guard<std::mutex> lock(mtx_context);
                        mode = client_context[thejob.client_id].mode;
                    }
                    auto temp = readFileBlock(thejob.fileName,0,mode);
                    unsigned char * packet = temp.first;
                    size_t data_size = temp.second;
                    if(data_size == 0){
                        // reached the end of file ==> Ack recv is the final Ack
                        continue; // to while loop
                    }
                    packet = build_data_packet(1,packet,data_size);
                    delete []temp.first;

                    // Send the message to the client
                    ssize_t sent_bytes = sendto(sockfd, packet, data_size+4, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
                    auto sent_timestamp = std::chrono::steady_clock::now();
                    check_err(sent_bytes,"sendto failed");

                    //debug:
                    std::cout<<"Inside Frwd Thread Just Sent Data of block1  in response to Read\n";

                    //building the job for timer thread
                    thejob.message = std::string(reinterpret_cast<char*>(packet), data_size+4);
                    thejob.mssg_size = data_size +4;
                    thejob.timestamp = sent_timestamp;
                    
                    free(packet);
                }
                    break;
                case 'W': // recv W req from client| send Ack 0 to client;
                {                        
                    // prepare packet to be sent
                    auto packet = build_ack_packet(0);

                    // Send the message to the client
                    ssize_t sent_bytes = sendto(sockfd, packet, 4, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
                    auto sent_timestamp = std::chrono::steady_clock::now();
                    check_err(sent_bytes,"sendto failed");

                    //debug:
                    std::cout<<"Inside Frwd Thread Just Sent Ack 0  in response to Write\n";
                    
                    //building the job for timer thread
                    thejob.message = std::string(reinterpret_cast<char*>(packet),4);
                    thejob.mssg_size = 4;
                    thejob.timestamp = sent_timestamp;

                    free(packet);
                        
                }
                    break;
                case 'D': // recved Data ==> Client is writing | save this data in file at offset blk-1 and send Ack of blk
                {
                    std::string mode;
                    {
                        std::lock_guard<std::mutex> lock(mtx_context);
                        mode = client_context[thejob.client_id].mode;
                    }
                    // save the data packet recv in file
                    writeFileBlock(thejob.fileName,thejob.block_num -1 ,mode,thejob.message,thejob.mssg_size);

                    // prepare ACK packet of blk_num = blk to be sent
                    auto packet = build_ack_packet(thejob.block_num);

                    // Send the message to the client
                    ssize_t sent_bytes = sendto(sockfd, packet, 4, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
                    auto sent_timestamp = std::chrono::steady_clock::now();
                    check_err(sent_bytes,"sendto failed");

                    //debug:
                    std::cout<<"Inside Frwd Thread Just Sent Ack for "<<thejob.block_num<<" th packet\n";
                    
                    //building the job for timer thread
                    thejob.message = std::string(reinterpret_cast<char*>(packet),4);
                    thejob.mssg_size = 4;
                    thejob.timestamp = sent_timestamp;

                    free(packet);
                }

                    break;
                case 'A': // recv Ack ==> Client is reading | send (Ack) th offset  data block(blk_num = ack+1)
                {
                    // prepare packet to be sent
                    std::string mode;
                    {
                        std::lock_guard<std::mutex> lock(mtx_context);
                        mode = client_context[thejob.client_id].mode;
                    }
                    auto temp = readFileBlock(thejob.fileName,thejob.block_num,mode);
                    unsigned char * packet = temp.first;
                    size_t data_size = temp.second;
                    if(data_size == 0){
                        // reached the end of file ==> Ack recv is the final Ack
                        continue; // to while loop
                    }
                    packet = build_data_packet(thejob.block_num+1,packet,data_size);
                    delete []temp.first;

                    // Send the message to the client
                    ssize_t sent_bytes = sendto(sockfd, packet, data_size+4, 0, (const struct sockaddr*)&client_addr, sizeof(client_addr));
                    auto sent_timestamp = std::chrono::steady_clock::now();
                    check_err(sent_bytes,"sendto failed");

                    //debug:
                    std::cout<<"Inside Frwd Thread Just Sent Data for "<<thejob.block_num+1<<" th packet\n";
                    

                    //building the job for timer thread
                    thejob.message = std::string(reinterpret_cast<char*>(packet), data_size+4);
                    thejob.mssg_size = data_size +4;
                    thejob.timestamp = sent_timestamp;

                    free(packet);                    
                }
                    break;
                case 'E': // recv error mssg
                    break;
                default :
                    std::cout<<" Received Wrong Packet Opcode\n";
                    exit(0);
            }
      

        }
  
        //updating the TimerQ;
        {
            std::lock_guard<std::mutex> lock(mtx_Timer);
            TimerQ.push_back(thejob);
            TimerM[thejob.client_id]= std::prev(TimerQ.end());
        }

        //debug
        std::cout<<"Added Job to TimerQ by Frwd thread\n";

        cv_timer.notify_one(); // notify the timer thread that TimeQ has data

    } // inf looping
}