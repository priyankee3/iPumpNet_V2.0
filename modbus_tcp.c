#include"header.h"

u16 modbus_tcp_buffer[256];
f32 vR,vY,vB,cR,cY,cB,frq,pfR,pfY,pfB;

// function to fetch data from MFM
bool modbus_tcp(const s8* ip, s32 portNo, s32 slaveID, u16 address, u16 size)
{	
	// Create new MODBUS TCP FD
	modbus_t *ctx = modbus_new_tcp(ip,portNo);

	// If failed to create FD
	if(ctx == NULL)
	{
		fprintf(stderr,"failed to create FD\n");
		return 0;
	}

	// Connected to Device
	if(modbus_connect(ctx) == -1)
	{
		fprintf(stderr,"unable to connect to device: %s\n",modbus_strerror(errno));
		return 0;
	}

	// Set slave ID
	modbus_set_slave(ctx,slaveID);

	// Read 2 registers from address 0x0000	
	int rc = modbus_read_registers(ctx,address, size, modbus_tcp_buffer);

	if(rc == -1)
	{
		fprintf(stderr,"Failed to read: %s\n",modbus_strerror(errno));
		modbus_close(ctx);
		modbus_free(ctx);
		return 0;
	}

	/*printf("buffer[0] = 0x%X\n",modbus_tcp_buffer[0]);
	  printf("buffer[1] = 0x%X\n",modbus_tcp_buffer[1]);
	  printf("buffer[2] = 0x%X\n",modbus_tcp_buffer[2]);
	  printf("buffer[3] = 0x%X\n",modbus_tcp_buffer[3]);*/

	modbus_close(ctx);
	modbus_free(ctx);

	return 1;
}

// Function to store and display data into Float variable
bool convert_to_F32(void)
{
	hex_float.value[0] = modbus_tcp_buffer[1] & 0xFF;
	hex_float.value[1] = (modbus_tcp_buffer[1] >> 8) & 0xFF;
	hex_float.value[2] = modbus_tcp_buffer[0] & 0xFF;
	hex_float.value[3] = (modbus_tcp_buffer[0] >>8) &0xFF;

	return 1;
}

// Function to fetch data from MFM EMpro 
void fetch_MFM(void)
{
	// Voltage Red
	if(modbus_tcp(MFM, MFM_PORTNO, 1, VR, 4))
	{
		convert_to_F32();
		printf("VR: %f V\t",hex_float.result);
		vR = hex_float.result;
		cJSON_AddNumberToObject(json_send, "VR", vR);
	}

	// Voltage Yellow
	if(modbus_tcp(MFM, MFM_PORTNO, 1, VY, 4))
	{
		convert_to_F32();
		printf("VY: %f V\t",hex_float.result);
		vY = hex_float.result;
		cJSON_AddNumberToObject(json_send, "VY", vY);
	}

	// Voltage Green
	if(modbus_tcp(MFM, MFM_PORTNO, 1, VB, 4))
	{
		convert_to_F32();
		printf("VB: %f V\t",hex_float.result);
		vB = hex_float.result;
		cJSON_AddNumberToObject(json_send, "VB", vB);
	}

	// Current Red
	if(modbus_tcp(MFM, MFM_PORTNO, 1, CR, 4))
	{
		convert_to_F32();
		printf("CR: %f A\t",hex_float.result);
		cR = hex_float.result;
		cJSON_AddNumberToObject(json_send, "CR", cR);
	}

	// Current Yellow
	if(modbus_tcp(MFM, MFM_PORTNO, 1, CY, 4))
	{
		convert_to_F32();
		printf("CY: %f A\t",hex_float.result);
		cY = hex_float.result;
		cJSON_AddNumberToObject(json_send, "CY", cY);
	}

	// Current Blue
	if(modbus_tcp(MFM, MFM_PORTNO, 1, CB, 4))
	{
		convert_to_F32();
		printf("CB: %f A\t",hex_float.result);
		cB = hex_float.result;
		cJSON_AddNumberToObject(json_send, "CB", cB);
	}

	printf("\n");

	// Power Factor Red
	if(modbus_tcp(MFM, MFM_PORTNO, 1, PFR, 4))
	{
		convert_to_F32();
		printf("PF Red: %f \t", hex_float.result);
		pfR = hex_float.result;
		cJSON_AddNumberToObject(json_send, "PFR", pfR);
	}
	
	// Power Factor Yellow
	if(modbus_tcp(MFM, MFM_PORTNO, 1, PFY, 4))
	{
		convert_to_F32();
		printf("PF Yellow: %f \t", hex_float.result);
		pfY = hex_float.result;
		cJSON_AddNumberToObject(json_send, "PFY", pfY);
	}
	
	// Power Factor blue
	if(modbus_tcp(MFM, MFM_PORTNO, 1, PFB, 4))
	{
		convert_to_F32();
		printf("PF Blue: %f \t", hex_float.result);
		pfB = hex_float.result;
		cJSON_AddNumberToObject(json_send, "PFB", pfB);
	}
	
	// Frequency
	if(modbus_tcp(MFM, MFM_PORTNO, 1, FRQ, 4))
	{
		convert_to_F32();
		printf("Frequncy: %f \t", hex_float.result);
		frq = hex_float.result;
	}

	// RPM
}
