/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** Main Program
******************************************************************/

#include "PacketConfig.h"
#include "EthernetPacket.h"
#include "ECPRIPacket.h"
#include "ORANPacket.h"
#include "PacketGenerator.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    // Ensure that exactly three command-line arguments are provided (config file, output file, and IQ file)
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <config_file> <output_file> <iq_file>" << endl;
        return 1;
    }

    string ConfigFile = argv[1]; // First argument: path to the configuration file
    string OutputFile = argv[2]; // Second argument: path to the output file where packets will be saved
    string IQFile = argv[3];     // Third argument: path to the file containing IQ samples

    // Create an instance of the PacketConfig class to manage packet settings
    PacketConfig Config;

    // Load packet configuration settings from the specified configuration file
    Config.SetConfig(ConfigFile);

    // Generate Ethernet packets
    PacketGenerator Generator(Config, OutputFile, IQFile);

    cout << "Ethernet packets generated and written to " << OutputFile << endl;

    return 0;
