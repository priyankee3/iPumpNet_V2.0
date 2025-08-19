#include"header.h"

u32 len;
FILE *fd_log = NULL;
// Function to handle the UDP clients
void * udp_handle(void * arg)	
{
	len = sizeof(cliaddr);
	while(1)
	{
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
			P1 = cJSON_GetObjectItem(json,"P1")->valuedouble;
			T2 = cJSON_GetObjectItem(json,"T2")->valuedouble;
			P2 = cJSON_GetObjectItem(json,"P2")->valuedouble;
			TStamp = cJSON_GetObjectItem(json,"TStamp")->valuestring;
			
			printf("Temperature T1:%f\t",T1);
			printf("Pressure P1:%f\t",P1);
			printf("Temperature T2:%f\t",T2);
			printf("Pressure P2:%f\t",P2);
			printf("Date & Time:%s\n",TStamp);
		}
		
		fetch_MFM();	// Fetching Data from MFM
		
		fd_log = fopen("../DataBase/Log.csv", "a");
		if( fd_log < 0 )
			perror("File Status:");
		
		fprintf(fd_log,"%s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",TStamp, T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB);
		fclose(fd_log);
		sendto(sockfd, "Message received", 16, 0, (const struct sockaddr*)&cliaddr, len);
	}
}
