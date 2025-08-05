#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <errno.h>
#include <stdbool.h>

// Type definitions
typedef char s8;
typedef unsigned char u8;
typedef short int s16;
typedef unsigned short int u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;
typedef double d64;

// External buffer
extern u16 modbus_tcp_buffer[260];

// Function prototype

bool modbus_tcp(const s8 *, s32, s32, u16, u16);	// Fetch data from MFM: IP address, port, slave ID, register address, number of registers
bool convert_to_F32(void);	// Function to convert recevied data and store it into float variable

// Macro
#define MFM "192.168.1.102"
#define MFM_PORTNO 502
#define VR 0x8006
#define VY 0x8008
#define VB 0x800A
#define CR 0x800E
#define CY 0x8010
#define CB 0x8012
#define FRQ 0x800c
#define PFR 0x8030
#define PFY 0x8032
#define PFB 0x8034

// Structure for float/integer conversion
union convert_hex_float{
    u8 value[4];
    f32 result;
};

extern union convert_hex_float hex_float;


#endif
