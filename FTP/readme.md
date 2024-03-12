# How to use The FTP server and client


## -------------- building the executables ----------

### Keep all the server.c common.c and makefile in a folder(X) in PC1

### Keep all the client.c common.c and makefile in a folder(Y) in PC2

### cd into respective folder(X/Y) and run "make" to build respective server.out and client.out files



## -------------- setup of server -------------

### Keep the server.out , id_passwd.txt in server computer (PC1) folder<X>

### id_passwd.txt contains username and passwords of multiple clients
 
##### entry in id_passwd.txt has the format <userName>/:<password>

### In the same folder<X> create empty directory of the same name as client user name for every user

### Run server by ./server.out <ServerPortNo>



## -------------- setup of client -------------

### only need the client.out file in the client computer folder<Y>

### Run client by cd into same folder<Y> and then ./client.out <ServerIP> <ServerPort>
 
