
// All the below function implementation is present in the utils.c file

// Define a structure to hold the parsed arguments
struct Arguments {
    char* interfaceName;
    int numberOfPackets;
    char* protocolName;
};

void printUsage(); // Prints the correct way to run fbi

struct Arguments parseArguments(int argc, char *argv[]);//for cli_parsing

int createRawSocket(const char* interfaceName,const char* protocolName);

void getMacIp(int sock_recv,char * interfaceName,unsigned char *Ip,unsigned char *Mac);

void writeHeaderToFile(const char *filename, const char *text,unsigned char *Ip,unsigned char * Mac,char * interfaceName);

void printPacket(FILE * fp,unsigned char *packet, int start, int end);

int checkMac(unsigned char* packet,const unsigned char *mac,int start);
