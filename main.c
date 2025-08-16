/*****************************************************************************************
1] Fetching data through MFM
2] Implemented Multi-Threading for using UDP server 
3] Implemented UDP server to capture Data from Data acquisiton hardware 
	3.1] Implemented JSON parsing 
	3.2] 
*****************************************************************************************/

#include"header.h"

union convert_hex_float hex_float;

s8 buffer[256];
s32 sockfd ,  n;
struct sockaddr_in servaddr, cliaddr;
cJSON *json = NULL;	//file descriptor for JSON

int main()
{
	pthread_t tid;	// for Creating thread and getting thread id
	
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
	
	//  Creating Thread for UPD
	if(pthread_create(&tid, NULL, udp_handle, NULL) != 0)
		close(sockfd);
	
	pthread_detach(tid);	// no need to Join
	
	while(1)
	{
		fetch_MFM();
		sleep(1);
	}

	close(sockfd);
	return 0;
}
