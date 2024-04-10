# Compiler
CC = gcc

# Source files
ACK_SRC = ack.c
ERR_SRC = err.c
RD_WR_SRC = rd_wr.c
DATA_SRC = data.c
CLIENT_SRC = client.c
CW = Cwrite.c
CR = Cread.c



# Object files
ACK_OBJ = $(ACK_SRC:.c=.o)
ERR_OBJ = $(ERR_SRC:.c=.o)
RD_WR_OBJ = $(RD_WR_SRC:.c=.o)
DATA_OBJ = $(DATA_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
CW_OBJ = $(CW:.c=.o)
CR_OBJ = $(CR:.c=.o)


# Executables
CLIENT_EXE = client


# Rules
all: $(CLIENT_EXE) 

%.o: %.c
	$(CC)  -c -o $@ $<

$(CLIENT_EXE): $(CLIENT_OBJ) $(ACK_OBJ) $(ERR_OBJ) $(RD_WR_OBJ) $(DATA_OBJ) $(CW_OBJ) $(CR_OBJ)
	$(CC)  -o $@ $^
	rm -f *.o
	mv $(CLIENT_EXE) ./Cli





