
OUT_FILES = server.out client.out

all: $(OUT_FILES)

server.out: server.c common.c
	    gcc server.c -o server.out

client.out: client.c common.c
	    gcc client.c -o client.out
	    
clear:
	rm -f $(OUT_FILES)	    	    	
