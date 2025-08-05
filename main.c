/*****************************************************************************************
1] Fetching data through MFM
*****************************************************************************************/

#include"header.h"

union convert_hex_float hex_float;

int main()
{
	while(1)
	{
		// Voltage Red
		if(modbus_tcp(MFM, MFM_PORTNO, 1, VR, 4))
		{
			convert_to_F32();
			printf("VR: %f V\t",hex_float.result);
		}
		
		// Voltage Yellow
		if(modbus_tcp(MFM, MFM_PORTNO, 1, VY, 4))
		{
			convert_to_F32();
			printf("VY: %f V\t",hex_float.result);
		}
		
		// Voltage Green
		if(modbus_tcp(MFM, MFM_PORTNO, 1, VB, 4))
		{
			convert_to_F32();
			printf("VB: %f V\t",hex_float.result);
		}
		
		// Current Red
		if(modbus_tcp(MFM, MFM_PORTNO, 1, CR, 4))
		{
			convert_to_F32();
			printf("CR: %f A\t",hex_float.result);
		}
		
		// Current Yellow
		if(modbus_tcp(MFM, MFM_PORTNO, 1, CY, 4))
		{
			convert_to_F32();
			printf("CY: %f A\t",hex_float.result);
		}
		
		// Current Blue
		if(modbus_tcp(MFM, MFM_PORTNO, 1, CB, 4))
		{
			convert_to_F32();
			printf("CB: %f A\t",hex_float.result);
		}

		printf("\n");

		// Power Factor Red
		if(modbus_tcp(MFM, MFM_PORTNO, 1, PFR, 4))
		{
			convert_to_F32();
			printf("PF Red: %f \t", hex_float.result);
		}

		// Power Factor Yellow
		if(modbus_tcp(MFM, MFM_PORTNO, 1, PFY, 4))
		{
			convert_to_F32();
			printf("PF Yellow: %f\t", hex_float.result);
		}

		// Power Factor Blue
		if(modbus_tcp(MFM, MFM_PORTNO, 1, PFB, 4))
		{
			convert_to_F32();
			printf("PF Blue: %f\t", hex_float.result);
		}

		// Frequency
		if(modbus_tcp(MFM, MFM_PORTNO, 1, FRQ, 4))
		{
			convert_to_F32();
			printf("Frequency: %f Hz\n", hex_float.result);
		}
		sleep(1);
	}

}
