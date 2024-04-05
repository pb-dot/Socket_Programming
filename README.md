# Socket Programming Repository Overview

Welcome to my Socket Programming GitHub repository! Here you'll find the following exciting software projects:

1. **FTP Server and Client**
2. **Chat Server and Client**
3. **myARPING**

## FTP Server and Client

### Description:
My custom FTP (File Transfer Protocol) server and client facilitate the transfer of any types of files between a server and a client over a network. The server hosts files and allows multiple clients to connect, upload, and download files over TCP socket.This code doesnt use the standard FTP prtocol. This code can only run on Linux (debian based distros).

### Features:
- **Upload and Download:** Allows users to upload files from the client to the server and download files from the server to the client.
- **Peek** Allows client to view 1st 1kB of his file on server
- **View & Remove** Allows clients to see the names of his files on server and remove them.
- **Security:** Implements  authentication  to ensure the confidentiality of transferred files.
- **Consistency** Server Maintains a Log file for every transaction

### Usage:
- **Server:** Run the FTP server on a designated machine. It will listen for incoming connections from clients.
- **Client:** Connect to the server using the client application. Navigate directories and transfer files as needed.

## Chat Server and Client

### Description:
The one to one chat server and client enable real-time communication between 2 end users over a network. Users can exchange messages over TCP socket

### Features:
- **Real-time Messaging:** Allows users to send and receive messages instantly.

### Usage:
- **Server:** Launch the chat server to act as end system.
- **Client:** Connect to the server using the client application to act as another end system.

## myArping
### How to run my custom arping command (returns Mac of target Ip):
- Copy all the .c files and the makefile in a folder
- cd into that folder
- sudo bash  // press enter
- make       // press enter
- ./app.out <iterface Name> <targetIp> // press enter
- <b>  Press ctrl+c to see results</b>

Feel free to contribute to our repository and enhance the software ecosystem!

Happy coding!
