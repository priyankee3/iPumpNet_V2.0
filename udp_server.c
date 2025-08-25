#include"header.h"

u32 len;
FILE *fd_log = NULL;
// Function to handle the UDP clients
void * udp_handle(void * arg)	
{
	bool log_file = 1;	// variable to select data will go in which file
	len = sizeof(cliaddr);
	while(1)
	{
		log_file = 1;
		n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
		buffer[n] = '\0';
		
		printf("Client: %s\n", buffer);
		// Paresing the JSON data
		json = cJSON_Parse(buffer);
		if( json == NULL )
		{
			printf("JSON Prasing Status: %s", cJSON_GetErrorPtr());
			cJSON_Delete(json);
		}
		else
		{
			T1 = cJSON_GetObjectItem(json,"T1")->valuedouble;
				if( T1 >= 0x0386 && T1 <= 0x0393 )  
					log_file = 0;
					
			P1 = cJSON_GetObjectItem(json,"P1")->valuedouble;
				if( P1 >= 0x0386 && P1 <= 0x0393 )  
					log_file = 0;

			T2 = cJSON_GetObjectItem(json,"T2")->valuedouble;
				if( T2 >= 0x0386 && T2 <= 0x0393 )  
					log_file = 0;

			P2 = cJSON_GetObjectItem(json,"P2")->valuedouble;
				if( P2 >= 0x0386 && P2 <= 0x0393 )  
					log_file = 0;

			TStamp = cJSON_GetObjectItem(json,"TStamp")->valuestring;
			
			printf("Temperature T1:%f\t",T1);
			printf("Pressure P1:%f\t",P1);
			printf("Temperature T2:%f\t",T2);
			printf("Pressure P2:%f\t",P2);
			printf("Date & Time:%s\n",TStamp);

			if(TStamp[10] == 'T')
				TStamp[10] = ' ';
		}
		
		fetch_MFM();	// Fetching Data from MFM
		
		if(log_file )
		{
			fd_log = fopen("../DataBase/Log.csv", "a");
			if( fd_log < 0 )
				perror("File Status:");
		
			fprintf(fd_log,"%s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",TStamp, T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB);
			fclose(fd_log);
		}
		else
		{
			fd_error_log = fopen("../DataBase/Error_log.csv", "a");
			if( fd_error_log < 0 )
				perror("File Status:");
		
			fprintf(fd_error_log,"%s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",TStamp, T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB);
			fclose(fd_error_log);
		}
		sendto(sockfd, "Message received", 16, 0, (const struct sockaddr*)&cliaddr, len);
	}
}
