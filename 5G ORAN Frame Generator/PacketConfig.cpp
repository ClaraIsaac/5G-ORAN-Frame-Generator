/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** Packet Configuration
** Source File
******************************************************************/

#include "PacketConfig.h"
#include <fstream>  // Required for file handling
#include <iostream> // For input/output operations
#include <algorithm> // For the remove_if algorithm
#include <cctype>   // For tolower

// Constructor for the EthernetPacketConfig class
PacketConfig::PacketConfig() {}

// Function to set packet configuration from a file
// It reads configuration parameters from the given file and stores them in the class members.
void PacketConfig::SetConfig(string& file_name)
{
    string line;           // Variable to store each line of the file
    size_t comment_index;  // Variable to store the index of any comment in the line
    size_t equal_index;    // Variable to store the index of the '=' character
    string key;            // Holds the parameter name
    string value;          // Holds the value associated with the parameter

    // Open the input file with the given file name
    ifstream file(file_name);

    // If the file cannot be opened, display an error message and exit the function
    if(!file)
    {
        cerr << "File could not be opened!\n";
        return;
    }

    // Read the file line by line
    while (getline(file, line))
    {
        // Remove any comment from the line (starting with //)
        comment_index = line.find("//");
        if (comment_index != string::npos)
        {
            line = line.substr(0, comment_index); // Keep the part before the comment
        }

        // Remove all spaces from the line
        line.erase(remove_if(line.begin(), line.end(), [](char c) { return c == ' '; }), line.end());

        // Find the position of the '=' character
        equal_index = line.find('=');
        if (equal_index == string::npos) continue;  // Skip the line if no '=' is found

        // Extract the key and value from the line
        key = line.substr(0, equal_index);          // Extract key (parameter name)
        value = line.substr(equal_index+1);         // Extract value

        if (key == "Eth.LineRate")
        {
            LineRate = stoi(value);  // Convert string to integer
        }
        else if (key == "Eth.CaptureSizeMs")
        {
            CaptureSizeMs = stoi(value);
        }
        else if (key == "Eth.MinNumOfIFGsPerPacket")
        {
            MinNumOfIFGsPerPacket = stoi(value);
        }
        else if (key == "Eth.DestAddress")
        {
            // Loop through the string in steps of 2 to extract each byte in hexadecimal
            for (int i = 2; i < 14; i=i+2)
            {
                DestAddress[i/2-1] = static_cast<unsigned char>(stoul(value.substr(i, 2), nullptr, 16));
            }
        }
        else if (key == "Eth.SourceAddress")
        {
            for (int i = 2; i < 14; i=i+2)
            {
                SourceAddress[i/2-1] = static_cast<unsigned char>(stoul(value.substr(i, 2), nullptr, 16));
            }
        }
        else if (key == "Eth.MaxPacketSize")
        {
            MaxPacketSize = stoi(value);
        }
        else if (key == "Oran.SCS")
        {
            SCS = stoi(value);
        }
        else if (key == "Oran.MaxNrb")
        {
            MaxNrb = stoi(value);
        }
        else if (key == "Oran.NrbPerPacket")
        {
            NrbPerPacket = stoi(value);
        }
        else if (key == "Oran.PayloadType")
        {
            string lowercasevalue;
            for (char& c : value)
                lowercasevalue += tolower(static_cast<unsigned char>(c));
            if (lowercasevalue == "random" || lowercasevalue == "fixed")
            {
                PayloadType = lowercasevalue;
            }
            else
            {
                cerr << "PayloadType has invalid argument!";
                return;
            }
        }
        else if (key == "Oran.Payload")
        {
            PayloadFile = value;
        }
    }

    // Close the file after reading all lines
    file.close();
    return;
}
