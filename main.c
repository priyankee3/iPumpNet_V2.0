/*****************************************************************************************
1] Fetching data through MFM
*****************************************************************************************/

#include"header.h"

union convert_hex_float hex_float;

int main()
{


	if(modbus_tcp(MFM, 502, 1, 32774, 2))
	{
		printf("Done\n");

		hex_float.value[0] = modbus_tcp_buffer[1] & 0xFF;
		hex_float.value[1] = (modbus_tcp_buffer[1] >> 8) & 0xFF;
		hex_float.value[2] = modbus_tcp_buffer[0] & 0xFF;
		hex_float.value[3] = (modbus_tcp_buffer[0] >> 8) & 0xFF ;

		printf("Voltage: %f\n",hex_float.result);

		for(int i = 3; i >= 0 ; i--)
			printf("%X ",hex_float.value[i]);

		printf("\n");
			
	}
	else
		printf("Not Done\n");

}
