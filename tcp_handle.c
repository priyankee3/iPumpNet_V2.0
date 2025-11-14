#include"header.h"

u32 len;
FILE *fd_log = NULL;
s8 *ip = "192.168.2.121";	// IP for MQTT broker
s8 *DSN;
s32 projectID;	// Project ID
s32 pumpSetID;	// PumpSet ID
s32 pumpNo;	// Pump Number

// Function to handle the UDP clients
void * tcp_handler(void * arg)	
{
	bool log_file = 1;	// variable to select data will go in which file
	len = sizeof(cliaddr);
	int connfd = *(int*)arg;
    	free(arg);
	while(1)
	{
		/******************** JSON File ********************/
		json_send = cJSON_CreateObject();	// Creating json object for send JSON file
		
		log_file = 1;
		n = recvfrom(connfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
		buffer[n] = '\0';
		
		printf("Client: %s\n", buffer);
		// Paresing the JSON data
		json_receive = cJSON_Parse(buffer);
		if( json_receive == NULL )
		{
			printf("JSON Prasing Status: %s\n", cJSON_GetErrorPtr());
			//cJSON_Delete(json_receive);
		}
		else
		{

			DSN = cJSON_GetObjectItem(json_receive,"DSN")->valuestring;
			cJSON_AddStringToObject(json_send,"DSN", DSN);
			
			projectID = (int) cJSON_GetObjectItem(json_receive,"Project_Id")->valuedouble;
			cJSON_AddNumberToObject(json_send,"Project_ID", projectID);

			pumpSetID = (int) cJSON_GetObjectItem(json_receive, "PumpSet_Id")->valuedouble;
			cJSON_AddNumberToObject(json_send, "PumpSet_ID", pumpSetID);
			
			pumpNo = (int) cJSON_GetObjectItem(json_receive, "pumpNo")->valuedouble;
			cJSON_AddNumberToObject(json_send, "PumpNo", pumpNo);
			
			TStamp = cJSON_GetObjectItem(json_receive,"TStamp")->valuestring;
			cJSON_AddStringToObject(json_send, "TStamp", TStamp);	

			T1 = cJSON_GetObjectItem(json_receive,"T1")->valuedouble;
				if( T1 >= 0x0386 && T1 <= 0x0393 )  
					log_file = 0;
			cJSON_AddNumberToObject(json_send, "T1", T1);
					
			P1 = cJSON_GetObjectItem(json_receive,"P1")->valuedouble;
				if( P1 >= 0x0386 && P1 <= 0x0393 )  
					log_file = 0;
			cJSON_AddNumberToObject(json_send, "P1", P1);

			T2 = cJSON_GetObjectItem(json_receive,"T2")->valuedouble;
				if( T2 >= 0x0386 && T2 <= 0x0393 )  
					log_file = 0;
			cJSON_AddNumberToObject(json_send, "T2", T2);

			P2 = cJSON_GetObjectItem(json_receive,"P2")->valuedouble;
				if( P2 >= 0x0386 && P2 <= 0x0393 )  
					log_file = 0;
			cJSON_AddNumberToObject(json_send, "P2", P2);

			
			printf("Temperature T1:%f\t",T1);
			printf("Pressure P1:%f\t",P1);
			printf("Temperature T2:%f\t",T2);
			printf("Pressure P2:%f\t",P2);
			printf("Date & Time:%s\n",TStamp);

			if(TStamp[10] == 'T')
				TStamp[10] = ' ';
		}
		
		//fetch_MFM();	// Fetching Data from MFM
		
		if(log_file )
		{
			fd_log = fopen("../DataBase/Log.csv", "a");
			if( fd_log < 0 )
				perror("File Status:");
		
			fprintf(fd_log,"%s, %d, %d, %d, %s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", DSN, projectID, pumpSetID, pumpNo, TStamp, T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB);
			fclose(fd_log);
		}
		else
		{
			fd_error_log = fopen("../DataBase/Error_log.csv", "a");
			if( fd_error_log < 0 )
				perror("File Status:");
		
			fprintf(fd_error_log,"%s, %d, %d, %d, %s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",DSN, projectID, pumpSetID, pumpNo, TStamp, T1, P1, T2, P2, vR, vY, vB, cR, cY, cB, frq, pfR, pfY, pfB);
			fclose(fd_error_log);
		}
		sendto(connfd, "Message received", 16, 0, (const struct sockaddr*)&cliaddr, len);
		
		log_file = 1;

		// Sending Data to MQTT Broker in JSON format
		mqtt_publish(ip);	
		
		cJSON_Delete(json_send);
	}
}
