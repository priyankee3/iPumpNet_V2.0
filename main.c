/*****************************************************************************************
1] Fetching data through MFM
2] Implemented Multi-Threading for using UDP server 
3] Implemented UDP server to capture Data from Data acquisiton hardware 
	3.1] Implemented JSON parsing 
	3.2] Log file and error log file implemented
*****************************************************************************************/

#include"header.h"

// Extern Variable declaration
union convert_hex_float hex_float;
struct mosquitto *mosq;

s8 buffer[256];
s32 sockfd ,  n;
struct sockaddr_in servaddr, cliaddr;
cJSON *json_receive = NULL;	//File descriptor for JSON to receive
cJSON *json_send = NULL;	//File descriptor for JSON to send
FILE *fd_error_log = NULL;	//file pointer for .csv file
f32 T1, P1, T2, P2;	// Variables for Temperature 1, 2 and Pressure 1, 2
s8 *TStamp = NULL;	// Variable for storing Time stamp

int main()
{
	s32 rc;
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
	servaddr.sin_addr.s_addr = inet_addr("192.168.2.120");	// For any IP of system
	servaddr.sin_port = htons(10051);

	if(bind( sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	printf("UDP server listening on port 10051.....\n");
	
	/******************** MQTT Inialization ********************/
	// Required before calling other mosquitto function
	mosquitto_lib_init();
	
	/* Create a new client instance
	   id = NULL -> ask the broker to generate a client id for us
	   clean session = true -> the broker should remove old sessions when we connect
	   obj = NULL -> we aren't passing any of our private data for call back */

	mosq = mosquitto_new(NULL, true, NULL);
	if( mosq == NULL )
	{
		fprintf(stderr,"Error in MQTT: Out of memory.\n");
	}
	
	// Configure callback. This should be done before connecting ideally
	mosquitto_connect_callback_set(mosq,on_connect);
	mosquitto_publish_callback_set(mosq,on_publish);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);
	
	// Enable automatic reconnect with backoff
	mosquitto_reconnect_delay_set(mosq, 2, 30, true);

	mosquitto_username_pw_set( mosq, "ee3", "Pump@2021" );
	/* Connect to Broker on port 1883, with a keepalive of 60 seconds.
	   This call make the socket connection only, it does not complete
	   the MQTT CONNECT/CONNACK flow, you should use mosquitto_loop_start()
	   or mosquitto_loop_forever() for processing net traffic */
	rc = mosquitto_connect(mosq, ip, 1883, 10);
	if(rc != MOSQ_ERR_SUCCESS)
	{
		//mosquitto_destroy(mosq);
		fprintf(stderr,"Error in MQTT: %s\n", mosquitto_strerror(rc));
		return 0;
	}

	// Runs network loop in a background thread, this calls returns quicky
	rc = mosquitto_loop_start(mosq);
	if(rc != MOSQ_ERR_SUCCESS)
	{
		printf("ok in publish\n");
		//mosquitto_destroy(mosq);
		fprintf(stderr,"Error in MQTT: %s\n", mosquitto_strerror(rc));
		return 0;
	}
	
	/* Publish Message 
	   mosq - our client instance
	   mid = NULL - we don't want to know what the message id for this message is 
	   topic = "test/topic" - the topic on which this message will be publish
	   payload - the actual payload
	   qos = 2 - publish with QoS for this example 
	   retain = false - do not use the retained message feature for this message */
	
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
		fprintf(fd_log,"DSN, Project_ID, PumpSet_ID, PumpNo., TStamp, Temperature 1, Pressure 1, Temperature 2, Pressure 2, Voltage Red, Voltage Yellow, Voltage Blue, Current Red, Current Yellow, Current Blue, Frequency, Power Factor Red, Power Factor Yellow, Power Factor Blue, RPM\n");
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
		fprintf(fd_log,"DSN, Project_ID, PumpSet_ID, PumpNo., TStamp, Temperature 1, Pressure 1, Temperature 2, Pressure 2, Voltage Red, Voltage Yellow, Voltage Blue, Current Red, Current Yellow, Current Blue, Frequency, Power Factor Red, Power Factor Yellow, Power Factor Blue, RPM\n");
		fclose(fd_log);
	}
	
	// Super Loop
	while(1)
	{
		sleep(1);
	}
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	
	cJSON_Delete(json_receive);
	close(sockfd);
	return 0;
}
