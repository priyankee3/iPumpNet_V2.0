#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include <modbus/modbus.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/socket.h>	// For creating socket
#include <arpa/inet.h>	// For converting IPs, Port no. to network order
#include <cjson/cJSON.h>	// For Transmitting and receving data in JSON format  
#include <pthread.h>	// For thread
#include <mosquitto.h>	// For Mosquitto MQTT

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
extern u16 modbus_tcp_buffer[256];
extern s8 buffer[256];	// For UDP 

// Function prototype
bool modbus_tcp(const s8 *, s32, s32, u16, u16);	// Fetch data from MFM: IP address, port, slave ID, register address, number of registers
bool convert_to_F32(void);	// Function to convert recevied data and store it into float variable
void fetch_MFM(void);	// Function to fetching values from MFM EMpro
void * udp_handle(void * arg);	// Function to handle Client on UDP Server	
void on_connect(struct mosquitto *, void *, int);	// Call back funciton for MQTT
void on_publish(struct mosquitto *, void *, int);	// Call back function for MQTT
bool mqtt_publish(s8 *);	// Function to send data to MQTT broker send ip address in string 
void on_disconnect( struct mosquitto *, void *, int );	//Callback on disconnect 

// Macro
#define MFM "192.168.2.102"
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

// Extern Variables
extern s8 buffer[256];
extern s32 sockfd, n;
extern u32 len;
extern struct sockaddr_in servaddr, cliaddr;
extern cJSON *json_receive;	//File descriptor for JSON to receive
extern cJSON *json_send;	// File descriptor for JSON to send
extern union convert_hex_float hex_float;
extern FILE *fd_log; // file descriptor for no error logs .csv file
extern FILE *fd_error_log;	// file descriptor for error log .csv file
extern f32 T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB;	// Variables for Temperature 1, 2 and Pressure 1, 2 and values from MFM 
extern s8 *TStamp;	// Variable for storing Time stamp
extern s8 *ip;
extern s8 *DSN;
extern s32 projectID;	// Project ID
extern s32 pumpSetID;	// PumpSet ID
extern s32 pumpNo;	// Pump Number
extern struct mosquitto *mosq;	// For MQTT Protocol

// Variable to Test 

#endif
