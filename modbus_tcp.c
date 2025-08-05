#include"header.h"

u16 modbus_tcp_buffer[260];

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
