/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** Ethernet Packet
** Header File
******************************************************************/

// Include guards to prevent multiple inclusions of this header file
#ifndef ETHERNET_PACKET_H
#define ETHERNET_PACKET_H

#include "ECPRIPacket.h" // Includes the ECPRI packet definition
#include "ORANPacket.h"  // Includes the ORAN packet definition
#include <vector>        // Required for vector

using namespace std;

// Constant for the Inter-Frame Gap (IFG) symbol used in Ethernet packets
const unsigned char IFGSymbol = 0x07;

// EthernetPacket class declaration
class EthernetPacket
{
public:
    // Preamble of the Ethernet packet (7 bytes) and Start Frame Delimiter (SDF, 1 byte)
    unsigned char Preamble[7] = {0xfb, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
    unsigned char SDF = 0xd5; // Start Frame Delimiter

    // Destination and Source MAC addresses (6 bytes each)
    unsigned char DestAddress[6];
    unsigned char SourceAddress[6];

    // EtherType field (2 bytes)
    unsigned char EtherType[2];

    // eCPRI and ORAN packet components
    ECPRIPacket ECPRI;
    ORANPacket ORAN;

    // CRC field for error detection (4 bytes)
    unsigned char CRC[4];

    // Inter-Frame Gap (IFG), variable length
    vector<unsigned char> IFG;

    // Payload size in bytes
    int PayloadSize;

    // Default constructor
    EthernetPacket() {}

    // Parameterized constructor for initializing Ethernet packet with input values
    EthernetPacket(unsigned char i_DestAddress[6], unsigned char i_SourceAddress[6], int i_PacketSize,
                   int i_MinNumOfIFGsPerPacket, ECPRIPacket i_ECPRI, ORANPacket i_ORAN);

    // Method to calculate the total size of the Ethernet packet (including payload and IFG)
    int Size();

private:
    // Method to align the total packet size to a multiple of 4 bytes
    void Align_4_Byte(int TotalSizeWithIFG);

    // Method to calculate the CRC for the payload
    void CRCCalculation();
};

// End of include guard
#endif //ETHERNET_PACKET_H
