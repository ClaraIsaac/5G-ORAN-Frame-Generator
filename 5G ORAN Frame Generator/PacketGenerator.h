/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** Packet Generator
** Header File
******************************************************************/

// Include guards to prevent multiple inclusions of this header file
#ifndef PACKET_GENERATOR_H
#define PACKET_GENERATOR_H

#include "PacketConfig.h"   // Include the PacketConfig class definition
#include "EthernetPacket.h" // Include the EthernetPacket class definition
#include "ECPRIPacket.h"    // Include the ECPRIPacket class definition
#include "ORANPacket.h"     // Include the ORANPacket class definition
#include <string>
#include <fstream>

using namespace std;

// Class for generating Ethernet packets based on configuration and packet details
class PacketGenerator
{
public:
    PacketConfig Config; // Configuration settings for packet generation

    // Constructor to initialize the PacketGenerator with configuration, output, and IQ file names
    PacketGenerator(const PacketConfig& i_Config, string& output_file, string& iq_file);

private:
    // Function to generate packets and write them to the specified output file
    void GeneratePackets(string& output_file, string& iq_file);

    // Function to send a single Ethernet packet to the output file
    void SendPacket(ofstream& file, EthernetPacket Packet);

    // Function to send Inter-Frame Gaps (IFGs) to the output file
    // NumOfIFGs specifies the number of IFG symbols to be sent
    void SendIFG(double NumOfIFGs, ofstream& file);
};

// End of include guard
#endif // PACKET_GENERATOR_H
