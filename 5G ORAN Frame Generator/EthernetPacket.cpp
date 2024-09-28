/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 19/09/2024
** Final Project, Milestone 1
** Ethernet Packet
** Source File
******************************************************************/

#include "EthernetPacket.h" // Include the EthernetPacket class definition
#include <iostream>

// CRC polynomial used in Ethernet CRC calculation (standard for Ethernet frames)
const uint32_t CRCPolynomial = 0x04C11DB7;

// Parameterized constructor for EthernetPacket class
EthernetPacket::EthernetPacket(unsigned char i_DestAddress[6], unsigned char i_SourceAddress[6], int i_PacketSize, int i_MinNumOfIFGsPerPacket, ECPRIPacket i_ECPRI, ORANPacket i_ORAN)
{
    ECPRI = i_ECPRI;
    ORAN = i_ORAN;

    // Calculate the payload size based on the packet size minus fixed Ethernet header fields
    PayloadSize = i_PacketSize - 26;

    // Initialize the destination and source MAC addresses
    for(int i = 0; i < 6; i++)
    {
        DestAddress[i]   = i_DestAddress[i];
        SourceAddress[i] = i_SourceAddress[i];
    }

    // Initialize the Inter-Frame Gap (IFG) with the specified number of IFG symbols
    for(int i = 0; i < i_MinNumOfIFGsPerPacket; i++)
    {
        IFG.push_back(IFGSymbol);
    }

    // Set the EtherType field to represent the payload size
    EtherType[0] = static_cast<unsigned char>(PayloadSize & 0xFF);        // Low byte
    EtherType[1] = static_cast<unsigned char>((PayloadSize >> 8) & 0xFF); // High byte

    // Calculate the CRC for the packet
    CRCCalculation();

    // Align the packet size to a multiple of 4 bytes by adding IFG symbols if necessary
    Align_4_Byte(i_PacketSize + i_MinNumOfIFGsPerPacket);
}

// Function to align the total packet size to a multiple of 4 bytes
void EthernetPacket::Align_4_Byte(int TotalSizeWithIFG)
{
    // If the total size is not a multiple of 4, add IFG symbols to align it
    if (TotalSizeWithIFG % 4 != 0)
    {
        for (int i = 0; i < TotalSizeWithIFG % 4; i++)
            IFG.push_back(IFGSymbol);
    }
}

// Function to calculate the CRC for the Ethernet packet
void EthernetPacket::CRCCalculation()
{
    // Initialize CRC to all ones (0xFFFFFFFF), as per standard CRC-32 initialization
    uint32_t CRC_temp = 0xffffffff;

    // Process each byte of the destination address (6 bytes)
    for (int i = 0; i < 6; ++i)
    {
        // XOR each byte of the destination address into the top 8 bits of the CRC
        CRC_temp ^= static_cast<uint32_t>(DestAddress[i]) << 24;

        // For each byte, process each bit
        for (int j = 0; j < 8; ++j)
        {
            // If the top bit is set (1), shift left and XOR with the polynomial
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                // If the top bit is 0, just shift left
                CRC_temp <<= 1;
            }
        }
    }

    // Process each byte of the source address (6 bytes)
    for (int i = 0; i < 6; ++i)
    {
        // XOR each byte of the source address into the top 8 bits of the CRC
        CRC_temp ^= static_cast<uint32_t>(SourceAddress[i]) << 24;

        // For each byte, process each bit
        for (int j = 0; j < 8; ++j)
        {
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                CRC_temp <<= 1;
            }
        }
    }

    // Process each byte of the EtherType field (2 bytes)
    for (int i = 0; i < 2; ++i)
    {
        // XOR each byte of the EtherType field into the top 8 bits of the CRC
        CRC_temp ^= static_cast<uint32_t>(EtherType[i]) << 24;

        for (int j = 0; j < 8; ++j)
        {
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                CRC_temp <<= 1;
            }
        }
    }

    // Process each byte of the ECPRI header, starting with the first byte (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ECPRI.HeaderFirstByte) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ECPRI message field (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ECPRI.Message) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ECPRI payload (2 bytes)
    for (int i = 0; i < 2; i++)
    {
        CRC_temp ^= static_cast<uint32_t>(ECPRI.Payload[i]) << 24;

        for (int j = 0; j < 8; ++j)
        {
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                CRC_temp <<= 1;
            }
        }
    }

    // Process the ECPRI PC_RTC field (2 bytes)
    for (int i = 0; i < 2; i++)
    {
        CRC_temp ^= static_cast<uint32_t>(ECPRI.PC_RTC[i]) << 24;

        for (int j = 0; j < 8; ++j)
        {
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                CRC_temp <<= 1;
            }
        }
    }

    // Process the ECPRI sequence ID (2 bytes)
    CRC_temp ^= static_cast<uint32_t>(ECPRI.SeqId) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN header (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.HeaderFirstByte) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN Frame ID (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.FrameId) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN SubFrameID_SlotID (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.SubFrameID_SlotID) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN SlotID_SymbolId (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.SlotID_SymbolId) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN SectionId (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.SectionId) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN SectionId_Rb_SymInc_StartPrbu (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.SectionId_Rb_SymInc_StartPrbu) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN StartPrbu (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.StartPrbu) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process the ORAN NumPrbu (1 byte)
    CRC_temp ^= static_cast<uint32_t>(ORAN.NumPrbu) << 24;

    for (int j = 0; j < 8; ++j)
    {
        if (CRC_temp & 0x80000000)
        {
            CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
        }
        else
        {
            CRC_temp <<= 1;
        }
    }

    // Process each IQ sample in the ORAN payload
    for (int i = 0; i < ORAN.IQSamples.size(); i++)
    {
        CRC_temp ^= static_cast<uint32_t>(ORAN.IQSamples[i]) << 24;

        for (int j = 0; j < 8; ++j)
        {
            if (CRC_temp & 0x80000000)
            {
                CRC_temp = (CRC_temp << 1) ^ CRCPolynomial;
            }
            else
            {
                CRC_temp <<= 1;
            }
        }
    }

    // Store the final CRC value in the CRC array
    CRC[0] = static_cast<unsigned char>(CRC_temp & 0xFF);
    CRC[1] = static_cast<unsigned char>((CRC_temp >> 8) & 0xFF);
    CRC[2] = static_cast<unsigned char>((CRC_temp >> 16) & 0xFF);
    CRC[3] = static_cast<unsigned char>((CRC_temp >> 24) & 0xFF);
}

// Function to calculate the total size of the Ethernet packet including payload and IFG
int EthernetPacket::Size()
{
    return 26 + PayloadSize + IFG.size(); // Fixed header size + payload size + IFG size
}
