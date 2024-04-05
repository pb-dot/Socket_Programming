#include "imports.h"
#include "protocol_header.h"
#include "utility_header.h"


struct tcpReturn icmp(FILE *fp, unsigned char *packet) {

    fprintf(fp,"\nNetwork Layer Protocol Analysis of the above packet\n");
    fprintf(fp,"\t:Icmp Header\n");

   uint8_t type = packet[0]; // Type field
    uint8_t code = packet[1]; // Code field
    uint16_t checksum = (packet[2] << 8) | packet[3]; // Checksum field

    fprintf(fp, "ICMP Header:\n");
    fprintf(fp, "  Type: %u\n", type);
    fprintf(fp, "  Code: %u\n", code);
    fprintf(fp, "  Checksum: 0x%04X\n", checksum);

    switch (type) {
        case 0: // Echo Reply
            fprintf(fp, "  ICMP Type: Echo Reply\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            break;
        case 3: // Destination Unreachable
            fprintf(fp, "  ICMP Type: Destination Unreachable\n");
            switch (code) {
                case 0:
                    fprintf(fp, "  ICMP Code: Network Unreachable\n");
                    break;
                case 1:
                    fprintf(fp, "  ICMP Code: Host Unreachable\n");
                    break;
                case 2:
                    fprintf(fp, "  ICMP Code: Protocol Unreachable\n");
                    break;
                case 3:
                    fprintf(fp, "  ICMP Code: Port Unreachable\n");
                    break;
                case 4:
                    fprintf(fp, "  ICMP Code: Fragmentation Needed and DF Set\n");
                    break;
                case 5:
                    fprintf(fp, "  ICMP Code: Source Route Failed\n");
                    break;
                 break;
                default:
                    fprintf(fp, "  ICMP Code: Unknown\n");
                    break;
            }
            // Remaining fields of Destination Unreachable
            // Remaining fields of Destination Unreachable
            fprintf(fp, "  Unused: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Next-Hop MTU: %02X%02X\n", packet[6], packet[7]);
            // Add more fields if needed
            // Add more fields if needed
            break;
        case 4: // Source Quench
            fprintf(fp, "  ICMP Type: Source Quench\n");
            fprintf(fp, "  Unused: %02X%02X%02X%02X\n", packet[4], packet[5], packet[6], packet[7]); // 4 bytes of Unused
            fprintf(fp, "  Original IP Header:\n");
            fprintf(fp, "    Version: %u\n", (packet[8] >> 4) & 0x0F);
            fprintf(fp, "    Header Length: %u bytes\n", (packet[8] & 0x0F) * 4);
            fprintf(fp, "    Type of Service: 0x%02X\n", packet[9]);
            fprintf(fp, "    Total Length: %u\n", (packet[10] << 8) | packet[11]);
            fprintf(fp, "    Identification: 0x%02X%02X\n", packet[12], packet[13]);
            fprintf(fp, "    Flags: 0x%02X\n", (packet[14] >> 5) & 0x07);
            fprintf(fp, "    Fragment Offset: %u\n", ((packet[14] & 0x1F) << 8) | packet[15]);
            fprintf(fp, "    Time to Live: %u\n", packet[16]);
            fprintf(fp, "    Protocol: %u\n", packet[17]);
            fprintf(fp, "    Header Checksum: 0x%04X\n", (packet[18] << 8) | packet[19]);
            fprintf(fp, "    Source IP Address: %u.%u.%u.%u\n", packet[20], packet[21], packet[22], packet[23]);
            fprintf(fp, "    Destination IP Address: %u.%u.%u.%u\n", packet[24], packet[25], packet[26], packet[27]);
            break;
        case 5: // Redirect Message
            fprintf(fp, "  ICMP Type: Redirect Message\n");
            switch (code) {
                case 0:
                    fprintf(fp, "  ICMP Code: Redirect Datagram for the Network\n");
                    break;
                case 1:
                    fprintf(fp, "  ICMP Code: Redirect Datagram for the Host\n");
                    break;
                case 2:
                    fprintf(fp, "  ICMP Code: Redirect Datagram for the Type of Service and Network\n");
                    break;
                case 3:
                    fprintf(fp, "  ICMP Code: Redirect Datagram for the Type of Service and Host\n");
                    break;
                default:
                    fprintf(fp, "  ICMP Code: Unknown\n");
                    break;
            }
            fprintf(fp, "  Gateway IP Address: %u.%u.%u.%u\n", packet[4], packet[5], packet[6], packet[7]);
            // Remaining fields of Redirect Message
            // Add more fields if needed
            break;
        case 8: // Echo Request
            fprintf(fp, "  ICMP Type: Echo Request\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            // Add more fields if needed
            break;
        case 11: // Time Exceeded
            fprintf(fp, "  ICMP Type: Time Exceeded\n");
            switch (code) {
                case 0:
                    fprintf(fp, "  ICMP Code: Time to Live exceeded in transit\n");
                    break;
                case 1:
                    fprintf(fp, "  ICMP Code: Fragment reassembly time exceeded\n");
                    break;
                default:
                    fprintf(fp, "  ICMP Code: Unknown\n");
                    break;
            }
            // Remaining fields of Time Exceeded
            fprintf(fp, "  Unused: %02X%02X%02X%02X\n", packet[4], packet[5], packet[6], packet[7]); // 4 bytes of Unused
            fprintf(fp, "  Original IP Header:\n");
            fprintf(fp, "    Version: %u\n", (packet[8] >> 4) & 0x0F);
            fprintf(fp, "    Header Length: %u bytes\n", (packet[8] & 0x0F) * 4);
            fprintf(fp, "    Type of Service: 0x%02X\n", packet[9]);
            fprintf(fp, "    Total Length: %u\n", (packet[10] << 8) | packet[11]);
            fprintf(fp, "    Identification: 0x%02X%02X\n", packet[12], packet[13]);
            fprintf(fp, "    Flags: 0x%02X\n", (packet[14] >> 5) & 0x07);
            fprintf(fp, "    Fragment Offset: %u\n", ((packet[14] & 0x1F) << 8) | packet[15]);
            fprintf(fp, "    Time to Live: %u\n", packet[16]);
            fprintf(fp, "    Protocol: %u\n", packet[17]);
            fprintf(fp, "    Header Checksum: 0x%04X\n", (packet[18] << 8) | packet[19]);
            fprintf(fp, "    Source IP Address: %u.%u.%u.%u\n", packet[20], packet[21], packet[22], packet[23]);
            fprintf(fp, "    Destination IP Address: %u.%u.%u.%u\n", packet[24], packet[25], packet[26], packet[27]);
            // Add more fields if needed
            break;
        case 12: // Parameter Problem
            fprintf(fp, "  ICMP Type: Parameter Problem\n");
            switch (code) {
                case 0:
                    fprintf(fp, "  ICMP Code: Pointer indicates the error\n");
                    break;
                case 1:
                    fprintf(fp, "  ICMP Code: Missing a required option\n");
                    break;
                case 2:
                    fprintf(fp, "  ICMP Code: Bad length\n");
                    break;
                default:
                    fprintf(fp, "  ICMP Code: Unknown\n");
                    break;
            }
            fprintf(fp, "  Pointer: %u\n", packet[4]);
            // Remaining fields of Parameter Problem
            fprintf(fp, "  Unused: %02X%02X\n", packet[5], packet[6]);
            // Add more fields if needed
            break;
        case 13: // Timestamp Request
            fprintf(fp, "  ICMP Type: Timestamp Request\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            fprintf(fp, "  Originate Timestamp: %02X%02X%02X%02X\n", packet[8], packet[9], packet[10], packet[11]);
            fprintf(fp, "  Receive Timestamp: %02X%02X%02X%02X\n", packet[12], packet[13], packet[14], packet[15]);
            fprintf(fp, "  Transmit Timestamp: %02X%02X%02X%02X\n", packet[16], packet[17], packet[18], packet[19]);
            // Add more fields if needed
            break;
        case 14: // Timestamp Reply
            fprintf(fp, "  ICMP Type: Timestamp Reply\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            fprintf(fp, "  Originate Timestamp: %02X%02X%02X%02X\n", packet[8], packet[9], packet[10], packet[11]);
            fprintf(fp, "  Receive Timestamp: %02X%02X%02X%02X\n", packet[12], packet[13], packet[14], packet[15]);
            fprintf(fp, "  Transmit Timestamp: %02X%02X%02X%02X\n", packet[16], packet[17], packet[18], packet[19]);
            // Add more fields if needed
            break;
        case 15: // Information Request
            fprintf(fp, "  ICMP Type: Information Request\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            // Add more fields if needed
            break;
        case 16: // Information Reply
            fprintf(fp, "  ICMP Type: Information Reply\n");
            fprintf(fp, "  Identifier: %02X%02X\n", packet[4], packet[5]);
            fprintf(fp, "  Sequence Number: %02X%02X\n", packet[6], packet[7]);
            // Add more fields if needed
            break;
        default:
            fprintf(fp, "  ICMP Type: Unknown\n");
            break;
    }
    
    struct tcpReturn ret;
    ret.src_port = 0;
    ret.dest_port = 0;
    ret.header_length = 0;
    return ret;
}


