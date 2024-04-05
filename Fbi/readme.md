
# FBI - Linux Packet Sniffer

FBI is a command-line interface (CLI) tool designed for packet sniffing on Linux systems. It captures network packets, performs detailed analysis on each packet's header layers, and outputs the results into three separate files based on the direction and type of packets. 

## Usage

To use FBI, execute the following command:


	./fbi -i interface_Name [-n number_Of_packets] [-p protocol_name]

## Options

    -i interface_Name: Specifies the network interface to sniff packets from.
    -n number_Of_packets: (Optional) Specifies the number of packets to capture. If not specified, it captures packets continuously until interrupted.
    -p protocol_name: (Optional) Filters packets based on the specified protocol.

## Output

FBI generates three output files:

    Packets Sent by User Machine to the Network: This file contains packets sent from the user's machine to the network. Each packet is presented in hexadecimal form along with detailed analysis of each layer header.

    Packets Sent to User's Machine from the Network: This file contains packets sent to the user's machine from the network. Similar to the previous file, it includes hexadecimal representation and detailed analysis of each layer header.

    Promiscuous Mode Packets: This file contains packets captured in promiscuous mode, where the network interface listens to all traffic on the network segment, regardless of the destination address. As with the other files, it provides hexadecimal representation and detailed header analysis.

## Filtering

Using the -p flag, users can filter packets based on the specified protocol. Only packets matching the provided protocol will be included in the output files.
Example

	eg ./fbi -i eth0 -n 1000 -p TCP

This command will capture 1000 TCP packets from the eth0 interface and output the results into the three files as described above.
Dependencies

FBI relies on standard Linux networking libraries and utilities for packet capture and analysis. No additional dependencies are required.

Contributors

    [Prithijit Banerjee]
    [Raj Shah]
    
