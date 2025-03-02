# Socket Programming Repository Overview

Welcome to my Socket Programming GitHub repository! Here you'll find the following exciting software projects:

1. **Highly Available Distributed File Transfer System**
2. **FBI : Linux Packet Sniffer**
3. **custom FTP Server and Client**
4. **standard TFTP Server and Client**
5. **one-to-one Chating app**
6. **myARPING -arping clone** 

<br><br><br>

## 1. Highly Available Distributed File Transfer System

<a href = "https://github.com/pb-dot/FTS">Click Here To Go this Repo</a>

<br><br><br>

## 2. FBI - Linux Packet Sniffer

FBI is a command-line interface (CLI) tool designed for packet sniffing on Linux systems. It captures network packets, performs detailed analysis on each packet's header layers, and outputs the results into three separate files based on the direction and type of packets. 

#### Usage

To use FBI, execute the following command:


	sudo ./fbi -i interface_Name [-n number_Of_packets] [-p protocol_name]

#### Options

    -i interface_Name: Specifies the network interface to sniff packets from.
    -n number_Of_packets: (Optional) Specifies the number of packets to capture. If not specified, it captures 10 packets.
    -p protocol_name: (Optional) Filters packets based on the specified protocol.

#### Output

FBI generates three output files:

    mySend_dd_mm_yyy_hh_min_sec.txt: This file contains packets sent from the user's machine to the network. Each packet is presented in hexadecimal form along with detailed analysis of each layer header.

    myRecv_dd_mm_yyy_hh_min_sec.txt: This file contains packets sent to the user's machine from the network. Similar to the previous file, it includes hexadecimal representation and detailed analysis of each layer header.

    Promiscuous_dd_mm_yyy_hh_min_sec.txt: This file contains packets captured in promiscuous mode, where the network interface listens to all traffic on the network segment, regardless of the destination address. As with the other files, it provides hexadecimal representation and detailed header analysis.

#### Filtering

Using the -p flag, users can filter packets based on the specified protocol. Only packets matching the provided protocol will be included in the output files.
Example

	eg sudo./fbi -i eth0 -n 1000 -p TCP

This command will capture 1000 TCP packets from the eth0 interface and output the results into the three files as described above.
Dependencies

FBI relies on standard Linux networking libraries and utilities for packet capture and analysis. No additional dependencies are required.

Contributors

    [Prithijit Banerjee]
    [Raj Shah]

<br><br><br>
    
## 3. custom FTP Server and Client

#### Description:
My custom FTP (File Transfer Protocol) server and client facilitate the transfer of any types of files between a server and a client over a network. The server hosts files and allows multiple clients to connect, upload, and download files over TCP socket.This code doesnt use the standard FTP prtocol. This code can only run on Linux (debian based distros).

#### Features:
- **Upload and Download:** Allows users to upload files from the client to the server and download files from the server to the client.
- **Peek** Allows client to view 1st 1kB of his file on server
- **View & Remove** Allows clients to see the names of his files on server and remove them.
- **Security:** Implements  authentication  to ensure the confidentiality of transferred files.
- **Consistency** Server Maintains a Log file for every transaction

#### Usage:
- **Server:** Run the FTP server on a designated machine. It will listen for incoming connections from clients.
- **Client:** Connect to the server using the client application. Navigate directories and transfer files as needed.

<br><br><br>

## 4. standard TFTP Server and Client
##### TFTP client : Compatible with any standard TFTP SERVER
##### TFTP server : Can Handle Multiple standard TFTP clients
##### File Transfer takes place in octect mode
#### How to run TFTP CLIENT :
- Copy all the .c files and the makefile in a folder
- cd into that folder (keep ur files to transfer in the Demo folder)
- make       // press enter
- cd ./build
- ./client_exe [Server IP] [ServerPort] r   // for reading files from server
- ./client_exe [Server IP] [ServerPort] w   // for writing files to server
#### How to run TFTP server (Run 1st) :
- Copy everything in a folder
- cd into that folder (keep ur files to transfer in the Demo folder)
- make       // press enter
- cd ./build
- ./server_exe [ServerPort]

<br><br><br>

## 5. one-to-one Chating app

#### Description:
The one to one chat server and client enable real-time communication between 2 end users over a network. Users can exchange messages over TCP socket.
One nend user uses the server app and the other uses the client app. 

#### Features:
- **Real-time Messaging:** Allows users to send and receive messages instantly.

#### Usage:
- **Server:** Launch the chat server to act as end system.
- **Client:** Connect to the server using the client application to act as another end system.

<br><br><br>

## 6. myArping : Ip to Mac Conversion (arpping clone)
#### How to run my custom arping command :
- Copy all the .c files and the makefile in a folder
- cd into that folder
- sudo bash  // press enter
- make       // press enter
- ./app.out <iterface Name> <targetIp> // press enter [iterface name can be wlo1 eth0]
- <b>  Press ctrl+c to see results</b>

