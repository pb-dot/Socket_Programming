
OUT_FILES = app.out

all: $(OUT_FILES)

app.out: arp_app.c arp_req.c arp_res.c imports.c
	    gcc arp_app.c -o app.out -lpthread
	    
clear:
	rm -f $(OUT_FILES)	    	    	
