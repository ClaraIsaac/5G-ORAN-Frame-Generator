/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** ORAN Packet
** Source File
******************************************************************/

#include "ORANPacket.h"

// Function to generate the ORAN packet by setting various header fields
void ORANPacket::GenerateORANPacket(unsigned char Frame_i, unsigned char Subframe_i,
                                    unsigned char Slot_i, unsigned char Symbol_i,
                                    unsigned int Section_i, unsigned int StartPrbu_i,
                                    unsigned char NumPrbu_i)
{
    // Set the Frame ID to the input value
    FrameId = Frame_i;

    // Combine the Subframe ID and Slot ID into a single byte
    SubFrameID_SlotID = ((Subframe_i << 4) & 0xF0) | ((Slot_i >> 2) & 0x0F);

    // Combine the Slot ID and Symbol ID into a single byte
    SlotID_SymbolId = ((Slot_i << 6) & 0xC0) | (Symbol_i & 0x3F);

    // Section ID is shifted to the right by 4 bits to fit into a single byte
    SectionId = Section_i >> 4;

    // Create the SectionId_Rb_SymInc_StartPrbu byte
    // Section ID is shifted to the left, and the StartPrbu upper bits are used in the lower bits
    // Setting SymInc and Rb bits to fixed values
    SectionId_Rb_SymInc_StartPrbu = ((Section_i << 4) & 0xF0) | (0x1 << 3) | (0x0 << 2) | ((StartPrbu_i >> 8) & 0x03);

    // Set the StartPrbu (lower 8 bits)
    StartPrbu = static_cast<unsigned char>(StartPrbu_i);

    // Set the number of PRBs
    NumPrbu = NumPrbu_i;
}
