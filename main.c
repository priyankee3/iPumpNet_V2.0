/*****************************************************************************************
1] Fetching data through MFM
2] Implemented Multi-Threading for using UDP server 
3] Implemented UDP server to capture Data from Data acquisiton hardware 
	3.1] Implemented JSON parsing 
	3.2] 
*****************************************************************************************/

#include"header.h"

// Extern Variable declaration
union convert_hex_float hex_float;

s8 buffer[256];
s32 sockfd ,  n;
struct sockaddr_in servaddr, cliaddr;
cJSON *json = NULL;	//file descriptor for JSON
FILE *fd_error_log = NULL;	//file pointer for .csv file
f32 T1, P1, T2, P2;	// Variables for Temperature 1, 2 and Pressure 1, 2
s8 *TStamp = NULL;	// Variable for storing Time stamp

int main()
{
	bool write_header = 1;	// Variable to write header or not
	pthread_t tid;	// for Creating thread and getting thread id
	
	/******************** IP and Port Initialization for communication ********************/

	// Create Socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);	// Socket Created for IPv4 , UDP
	if(sockfd < 0)
	{
		perror("Socket Status:");
		exit(EXIT_FAILURE);
	}

	// Binding server with information
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = INADDR_ANY;	// For any IP of system
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.120");	// For any IP of system
	servaddr.sin_port = htons(10051);

	if(bind( sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	printf("UDP server listening on port 10051.....\n");
	
	/******************** Thread Initialization ********************/
	
	//  Creating Thread for UPD
	if(pthread_create(&tid, NULL, udp_handle, NULL) != 0)
		close(sockfd);
	
	pthread_detach(tid);	// no need to Join
	
	/******************** File Inialization for .csv ********************/
	
	// For log file
	fd_log = fopen("../DataBase/Log.csv", "r");
	if(fd_log == NULL)
		write_header = 1;
	else
	{
		write_header = 0;
		fclose(fd_log);
	}

	fd_log = fopen("../DataBase/Log.csv","a");
	if( fd_log == NULL )
	{
		perror("File status:");
		return 1;
	}
	
	if(write_header)
	{
		fprintf(fd_log,"TStamp, Temperature 1, Pressure 1, Temperature 2, Pressure 2, Voltage Red, Voltage Yellow, Voltage Blue, Current Red, Current Yellow, Current Blue, Frequency, Power Factor Red, Power Factor Yellow, Power Factor Blue, RPM\n");
		fclose(fd_log);
	}

	//  For Error log file
	fd_error_log = fopen("../DataBase/Error_log.csv", "r");
	if(fd_error_log <= 0)
		write_header = 1;
	else
	{
		write_header = 0;
		fclose(fd_error_log);
	}

	fd_error_log = fopen("../DataBase/Error_log.csv", "a");
	if( fd_error_log <= 0 )
	{
		perror("Error log file status:");
		return 1;
	}
	
	if(write_header)
	{
		fprintf(fd_log,"TStamp, Temperature 1, Pressure 1, Temperature 2, Pressure 2, Voltage Red, Voltage Yellow, Voltage Blue, Current Red, Current Yellow, Current Blue, Frequency, Power Factor Red, Power Factor Yellow, Power Factor Blue, Angle of Voltage Phase 1, Angle of Voltage Phase 2, Angle of Voltage Phase 3\n");
		fclose(fd_log);
	}
	
	// Super Loop
	while(1)
	{
		sleep(1);
	}

	close(sockfd);
	return 0;
}
