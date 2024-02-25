/* mbed Microcontroller Library
 * Copyright (c) 2023 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#define PRINTF_TX PA_9
#define PRINTF_RX PA_10
static BufferedSerial forward_port(PRINTF_TX, PRINTF_RX, 115200);
FileHandle *mbed::mbed_override_console(int fd) {
    return &forward_port;
}

#define SERIAL_TX PA_2
#define SERIAL_RX PA_3
static BufferedSerial receive_port(SERIAL_TX, SERIAL_RX, 115200);

// Packet Definition
union{
    char bytes[8];
    struct{  // The struct and bytes[] share same memory location
        uint16_t start_seq; // 2 bytes
        float tf_range;     // 4 bytes
        uint16_t end_seq;   // 2 bytes
    }unpacked;
}packet;

uint16_t default_start_seq;

bool read()
{
    for (int i = 0; i < 2; i++)
    // Get first 2 bytes - start_seq - 16 bit 
    {
        receive_port.read(&packet.bytes[i], 1);
    }
    
    if(packet.unpacked.start_seq != default_start_seq)
    // Skips faulty data reading
    {
        return false;
    }

    for (int i = 2; i < 8; i++)
    // Read the remaining data sequence
    {
        receive_port.read(&packet.bytes[i], 1);
    }

    return true;
}

void send()
{


    for(int i=0; i<8; i++)
    {
        // send data byte by byte
        forward_port.write(&packet.bytes[i], 1);
    }

}

int main()
{

    default_start_seq = 0x0210;

    while (1)
    {

        if (receive_port.readable())
        {
            if (read())
            {
                // if read is sucsessful
                send();
            }
        }
        
    }
    
    return 0;
}