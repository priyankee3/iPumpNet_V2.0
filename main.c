/*****************************************************************************************
1] Fetching data through MFM
*****************************************************************************************/

#include"header.h"

union convert_hex_float hex_float;

int main()
{


	if(modbus_tcp(MFM, 502, 1, 0x8006, 4))
	{
		printf("Done\n");

		hex_float.value[0] = modbus_tcp_buffer[1];
		hex_float.value[1] = modbus_tcp_buffer[0];
		hex_float.value[2] = modbus_tcp_buffer[3];
		hex_float.value[3] = modbus_tcp_buffer[2];

		printf("Voltage: %f\n",hex_float.result);
	}
	else
		printf("Not Done\n");

}
