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
// Fetch data from MFM: IP address, port, slave ID, register address, number of registers
bool modbus_tcp(const s8 *, s32, s32, u16, u16);

// Macro
#define MFM "192.168.1.102"

// Structure for float/integer conversion
union convert_hex_float{
    u8 value[4];
    f32 result;
};

extern union convert_hex_float hex_float;


#endif
