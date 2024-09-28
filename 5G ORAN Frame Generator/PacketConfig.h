/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** Packet Configuration
** Header File
******************************************************************/

// Include guards to prevent multiple inclusions of this header file
#ifndef PACKET_CONFIG_H
#define PACKET_CONFIG_H

#include <string>

using namespace std;

// Declaration of the PacketConfig class
class PacketConfig
{
public:
    // Variables representing configuration parameters
    unsigned int LineRate;
    unsigned int CaptureSizeMs;
    unsigned int MinNumOfIFGsPerPacket;
    unsigned char DestAddress[6];
    unsigned char SourceAddress[6];
    unsigned int MaxPacketSize;
    unsigned int SCS;
    unsigned int MaxNrb;
    unsigned int NrbPerPacket;
    string PayloadType;
    string PayloadFile;


    // Constructor declaration
    PacketConfig();

    // Function to set the configuration from a configuration file
    void SetConfig(string& file_name);
};

// End of include guard
#endif // PACKET_CONFIG_H
