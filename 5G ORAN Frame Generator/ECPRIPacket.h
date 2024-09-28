/*****************************************************************
** Name: Clara Issa Ishac Abdelmessih
** Date: 28/09/2024
** Final Project, Milestone 2
** ECPRI Packet
** Header File
******************************************************************/

// Include guards to prevent multiple inclusions of this header file
#ifndef ECPRI_PACKET_H
#define ECPRI_PACKET_H

#include <vector>    // Required for vector

using namespace std;

class ECPRIPacket
{
public:
    unsigned char HeaderFirstByte = 0x00;
    unsigned char Message = 0x00; // User plane
    unsigned char Payload[2];
    unsigned char PC_RTC[2] = {0x00, 0x00};
    unsigned int SeqId;
};

// End of include guard
#endif //ECPRI_PACKET_H
