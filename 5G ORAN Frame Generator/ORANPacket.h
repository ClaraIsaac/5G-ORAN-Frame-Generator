/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** ORAN Packet
** Header File
******************************************************************/

// Include guards to prevent multiple inclusions of this header file
#ifndef ORAN_PACKET_H
#define ORAN_PACKET_H

#include <vector>  // Required for the vector type

using namespace std;

// Declaration of the ORANPacket class
class ORANPacket
{
public:
    unsigned char HeaderFirstByte = 0x00;  // First byte of the ORAN packet header
    unsigned char FrameId;                 // Frame ID, used to identify the frame number
    unsigned char SubFrameID_SlotID;       // Encoded Subframe ID and Slot ID (packed together into one byte)
    unsigned char SlotID_SymbolId;         // Encoded Slot ID and Symbol ID (packed into one byte)
    unsigned char SectionId;               // Section ID
    unsigned char SectionId_Rb_SymInc_StartPrbu; // Encoded Section ID, Resource Block, Symbol Increment, and Start PRB
    unsigned char StartPrbu;               // Start Physical Resource Block (PRB)
    unsigned char NumPrbu;                 // Number of PRBs in this packet
    vector<signed int> IQSamples;          // Vector of IQ samples representing the signal data

    // Default constructor
    ORANPacket() {}

    // Method to generate an ORAN packet by setting the necessary header fields
    void GenerateORANPacket(unsigned char Frame_i, unsigned char Subframe_i,
                            unsigned char Slot_i, unsigned char Symbol_i,
                            unsigned int Section_i, unsigned int StartPrbu_i,
                            unsigned char NumPrbu_i);
};

// End of include guard
#endif // ORAN_PACKET_H
