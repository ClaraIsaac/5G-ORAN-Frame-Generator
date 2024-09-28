/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 19/09/2024
** Final Project, Milestone 2
** Packet Generator
** Source File
******************************************************************/

#include "PacketGenerator.h"
#include <fstream>  // Required for file handling
#include <iostream>
#include <iomanip>  // For formatting output
#include <cmath>    // For ceil

// Constructor that initializes the config and generates packets
PacketGenerator::PacketGenerator (const PacketConfig& i_Config, string& output_file, string& iq_file)
{
    Config = i_Config;
    GeneratePackets(output_file, iq_file);
}

// Main function to generate the packets and write them to output
void PacketGenerator::GeneratePackets(string& output_file, string& iq_file)
{
    // Variables
    double LineRateGigaBytePerMicroSec = (Config.LineRate / 8.0) * 1000;
    int NumOfFrames = Config.CaptureSizeMs/10;
    int NumOfSlots = Config.SCS / 15;
    double SlotDurationUs = (1.0/NumOfSlots) * 1000;

    // 0 means 273
    if (Config.MaxNrb == 0)
        Config.MaxNrb =  273;
    if (Config.NrbPerPacket == 0)
        Config.NrbPerPacket = 273;

    int NumOfPacketsPerSymbol = ceil(static_cast<double>(Config.MaxNrb)/static_cast<double>(Config.NrbPerPacket));
    long PacketSize = 42 + (Config.NrbPerPacket * 12 * 2 * 2);
    char Fragment = 0, FirstFragment = 0;
    int NrbPerPacket1 = 0, NrbPerPacket2 = 0;
    double CurrentTime = 0;
    double TimePerPacket1 = 0, TimePerPacket2 = 0;
    int I, Q;

    // Initialize eCPRI packet
    ECPRIPacket ECPRI;
    ECPRI.SeqId = 0;

    // Open output file
    ofstream outfile(output_file);
    if (!outfile)
    {
        cerr << "File could not be opened!\n"; // Print error if file can't be opened
        return;
    }

    // Open IQ file for reading
    ifstream iqfile(iq_file);
    if (!iqfile)
    {
        cerr << "File could not be opened!\n"; // Print error if file can't be opened
        return;
    }

    // Handle fragmentation if packet size exceeds max allowed size
    if (PacketSize > Config.MaxPacketSize)
    {
        Fragment = 1;
        PacketSize = PacketSize - (2*12*2);
        NrbPerPacket2 ++;
        while (PacketSize > Config.MaxPacketSize)
        {
            PacketSize = PacketSize - (2*12*2);
            NrbPerPacket2 ++;
        }
        NrbPerPacket1 = Config.NrbPerPacket - NrbPerPacket2;

        // Calculate time per fragment
        TimePerPacket1 = static_cast<double>(42 + (NrbPerPacket1 * 12 * 2 * 2)) / LineRateGigaBytePerMicroSec;
        TimePerPacket2 = static_cast<double>(42 + (NrbPerPacket2 * 12 * 2 * 2)) / LineRateGigaBytePerMicroSec;
    }
    else
    {
        TimePerPacket1 = static_cast<double>(PacketSize) / LineRateGigaBytePerMicroSec;
        Fragment = 0;
    }

    // Loop over frames, subframes, slots, symbols, and generate packets
    for (int F = 0; F < NumOfFrames; F++)
    {
        for (int SF = 0; SF < 10; SF++)
        {
            for (int SL = 0; SL < NumOfSlots; SL++)
            {
                for (int SY = 0; SY < 14; SY++)
                {
                    int NrbUsed = 0;
                    for (int P = 0; P < NumOfPacketsPerSymbol; P++)
                    {
                        ORANPacket ORAN;

                        // Handle fragmented packets
                        if (Fragment && !FirstFragment)
                        {
                            ORAN.GenerateORANPacket(F, SF, SL, SY, P, NrbUsed, NrbPerPacket1);
                            for (int i = 0; i < NrbPerPacket1; i++)
                            {
                                for (int j = 0; j < 12; j++)
                                {
                                    // Handle payload based on fixed or random type
                                    if (Config.PayloadType == "fixed")
                                    {
                                        if (!(iqfile >> I >> Q))
                                        {
                                            // If the file ends, loop back to the start of the file
                                            iqfile.clear();
                                            iqfile.seekg(0, ios::beg);
                                            if (!(iqfile >> I >> Q))
                                            {
                                                cerr << "Error reading IQ file, even after looping back." << endl;
                                                break;
                                            }
                                        }
                                        ORAN.IQSamples.push_back(I);
                                        ORAN.IQSamples.push_back(Q);
                                    }
                                    else if (Config.PayloadType == "random")
                                    {
                                        ORAN.IQSamples.push_back(0);
                                        ORAN.IQSamples.push_back(0);
                                    }
                                }
                            }
                            FirstFragment = 1;
                            NrbUsed = NrbUsed + NrbPerPacket1; // Increment the number of resource blocks used
                            ECPRI.Payload[0] = static_cast<unsigned char>((8+(NrbPerPacket1*2*12*2)) & 0xFF);
                            ECPRI.Payload[1] = static_cast<unsigned char>(((8+(NrbPerPacket1*2*12*2)) >> 8) & 0xFF);

                            // Create the packet
                            EthernetPacket Ethernet(Config.DestAddress, Config.SourceAddress, 42 + (NrbPerPacket1 * 12 * 2 * 2), Config.MinNumOfIFGsPerPacket, ECPRI, ORAN);

                            // Write the packet in the file
                            SendPacket(outfile, Ethernet);

                            // Increment the current time
                            CurrentTime = CurrentTime + TimePerPacket1;

                            // Increment the sequence id
                            ECPRI.SeqId = (ECPRI.SeqId + 1) % 256;
                        }
                        else if (Fragment && FirstFragment)
                        {
                            ORAN.GenerateORANPacket(F, SF, SL, SY, P-1, NrbUsed, NrbPerPacket2);
                            for (int i = 0; i < NrbPerPacket2; i++)
                            {
                                for (int j = 0; j < 12; j++)
                                {
                                    // Handle payload based on fixed or random type
                                    if (Config.PayloadType == "fixed")
                                    {
                                        if (!(iqfile >> I >> Q))
                                        {
                                            // If the file ends, loop back to the start of the file
                                            iqfile.clear();
                                            iqfile.seekg(0, ios::beg);
                                            if (!(iqfile >> I >> Q))
                                            {
                                                cerr << "Error reading IQ file, even after looping back." << endl;
                                                break;
                                            }
                                        }
                                        ORAN.IQSamples.push_back(I);
                                        ORAN.IQSamples.push_back(Q);
                                    }
                                    else if (Config.PayloadType == "random")
                                    {
                                        ORAN.IQSamples.push_back(0);
                                        ORAN.IQSamples.push_back(0);
                                    }
                                }
                            }
                            FirstFragment = 0;
                            NrbUsed = NrbUsed + NrbPerPacket2; // Increment the number of resource blocks used
                            ECPRI.Payload[0] = static_cast<unsigned char>((8+(NrbPerPacket1*2*12*2)) & 0xFF);
                            ECPRI.Payload[1] = static_cast<unsigned char>(((8+(NrbPerPacket1*2*12*2)) >> 8) & 0xFF);


                            // Create the packet
                            EthernetPacket Ethernet(Config.DestAddress, Config.SourceAddress, 42 + (NrbPerPacket1 * 12 * 2 * 2), Config.MinNumOfIFGsPerPacket, ECPRI, ORAN);

                            // Write the packet in the file
                            SendPacket(outfile, Ethernet);

                            // Increment the current time
                            CurrentTime = CurrentTime + TimePerPacket2;

                            // Increment the sequence id
                            ECPRI.SeqId = (ECPRI.SeqId + 1) % 256;
                        }
                        else if (!Fragment)
                        {
                            if (NrbUsed + Config.NrbPerPacket > Config.MaxNrb)
                                ORAN.GenerateORANPacket(F, SF, SL, SY, P, NrbUsed, Config.MaxNrb - NrbUsed);
                            else
                                ORAN.GenerateORANPacket(F, SF, SL, SY, P, NrbUsed, Config.NrbPerPacket);
                            for (int i = 0; i < Config.NrbPerPacket; i++)
                            {
                                for (int j = 0; j < 12; j++)
                                {
                                    // Handle payload based on fixed or random type
                                    if (Config.PayloadType == "fixed")
                                    {
                                        if (!(iqfile >> I >> Q))
                                        {
                                            // If the file ends, loop back to the start of the file
                                            iqfile.clear();
                                            iqfile.seekg(0, ios::beg);
                                            if (!(iqfile >> I >> Q))
                                            {
                                                cerr << "Error reading IQ file, even after looping back." << endl;
                                                break;
                                            }
                                        }
                                        ORAN.IQSamples.push_back(I);
                                        ORAN.IQSamples.push_back(Q);
                                    }
                                    else if (Config.PayloadType == "random")
                                    {
                                        ORAN.IQSamples.push_back(0);
                                        ORAN.IQSamples.push_back(0);
                                    }
                                }
                            }
                            NrbUsed = NrbUsed + Config.NrbPerPacket; // Increment the number of resource blocks used
                            ECPRI.Payload[1] = static_cast<unsigned char>((8+(Config.NrbPerPacket*2*12*2)) & 0xFF);
                            ECPRI.Payload[0] = static_cast<unsigned char>(((8+(Config.NrbPerPacket*2*12*2)) >> 8) & 0xFF);

                            // Create the packet
                            EthernetPacket Ethernet(Config.DestAddress, Config.SourceAddress, 42 + (Config.NrbPerPacket * 12 * 2 * 2), Config.MinNumOfIFGsPerPacket, ECPRI, ORAN);

                            // Write the packet in the file
                            SendPacket(outfile, Ethernet);

                            // Increment the current time
                            CurrentTime = CurrentTime + TimePerPacket1;

                            // Increment the sequence id
                            ECPRI.SeqId = (ECPRI.SeqId + 1) % 256;
                        }
                    }
                }
                if (CurrentTime < SlotDurationUs * (SL+1))
                {
                    SendIFG(ceil(((SlotDurationUs * (SL+1)) - CurrentTime) * LineRateGigaBytePerMicroSec), outfile);
                }
            }
        }
    }

    // Close the IQ and output files
    iqfile.close();
    outfile.close();
}

// Send an Ethernet packet, formatted in hex, to the output file
void PacketGenerator::SendPacket(ofstream& file, EthernetPacket Packet)
{
    int firstbyte = 0;
    // Loop through each byte of the packet and write it to the file in hexadecimal format
    for (int i = 0; i < Packet.Size(); ++i)
    {
        // Insert a newline every 4 bytes
        if (i > 0 && i % 4 == 0)
        {
            file << '\n';
        }

        // Write different parts of the packet
        if (i < 7)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.Preamble[i]); // Write Preamble
        else if (i < 8)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.SDF); // Write Start Frame Delimiter (SDF)
        else if (i < 14)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.DestAddress[i - 8]); // Write Destination MAC Address
        else if (i < 20)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.SourceAddress[i - 14]); // Write Source MAC Address
        else if (i < 22)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.EtherType[i - 20]); // Write EtherType field
        else if (i < 23)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.HeaderFirstByte); // Write Payload
        else if (i < 24)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.Message); // Write Payload
        else if (i < 26)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.Payload[i - 24]); // Write Payload
        else if (i < 28)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.PC_RTC[i - 26]); // Write Payload
        else if (i < 29)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.SeqId >> 8); // Write Payload
        else if (i < 30)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ECPRI.SeqId); // Write Payload
        else if (i < 31)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.HeaderFirstByte); // Write Payload
        else if (i < 32)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.FrameId); // Write Payload
        else if (i < 33)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.SubFrameID_SlotID); // Write Payload
        else if (i < 34)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.SlotID_SymbolId); // Write Payload
        else if (i < 35)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.SectionId); // Write Payload
        else if (i < 36)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.SectionId_Rb_SymInc_StartPrbu); // Write Payload
        else if (i < 37)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.StartPrbu); // Write Payload
        else if (i < 38)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.NumPrbu); // Write Payload
        else if (i < 38 + Packet.ORAN.IQSamples.size()*2)
        {
            if (!firstbyte)
            {
                file << hex << setw(2) << setfill('0') << static_cast<int>((Packet.ORAN.IQSamples[(i - 38)/2] >> 8) &  0x00FF); // Write Payload
                firstbyte = 1;
            }
            else if (firstbyte)
            {
                file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.ORAN.IQSamples[(i - 1 - 38)/2] & 0x00FF);
                firstbyte = 0;
            }
        }
        else if (i < 38 + Packet.ORAN.IQSamples.size()*2 + 4)
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.CRC[i - (38 + Packet.ORAN.IQSamples.size())*2]); // Write CRC
        else
            file << hex << setw(2) << setfill('0') << static_cast<int>(Packet.IFG[i - (38 + Packet.ORAN.IQSamples.size()*2 + 4)]); // Write Inter-Frame Gap (IFG)
    }
    file << '\n';  // Insert a final newline after the entire packet
}

// Function to write the Inter-Frame Gap (IFG) to the output file
void PacketGenerator::SendIFG(double NumOfIFGs, ofstream& file)
{
    // Write the specified number of IFG symbols to the file
    for (size_t i = 0; i < NumOfIFGs; ++i)
    {
        // Insert a newline every 4 bytes for better readability
        if (i > 0 && i % 4 == 0)
        {
            file << '\n';
        }
        // Write the IFG symbol in hexadecimal format
        file << hex << setw(2) << setfill('0') << static_cast<int>(IFGSymbol);
    }
    file << '\n';  // Insert a final newline after writing the IFG
}
