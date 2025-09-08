#include"header.h"
bool mqtt_publish(s8 *ip)
{
	s32 rc;
	s8* message = NULL;	
	
	
	message = cJSON_Print(json_send);

	rc = mosquitto_publish(mosq,NULL, "THD/Data", strlen(message), message, 0, false );
	if(rc != MOSQ_ERR_SUCCESS)
	{
		//mosquitto_destroy(mosq);
		fprintf(stderr,"Error in MQTT: %s\n", mosquitto_strerror(rc));
		free(message);
		return 0;
	}	

	free(message);
	return 1;
}

// Callback called when the client receive a CONNACK messafe from the broker
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	// Print the connection result 
	printf("on_connect: %s\n",mosquitto_connack_string(reason_code));
	if(reason_code != 0)
	{
		/* if the connection fails for any reason, we don't want to keep 
		   on retrying in this example, so disconnect, without this, the
		   client will attempt to reconnect */
		mosquitto_disconnect(mosq);
	}
}

/* Callback called when the client knows to the best of its abilites that PUBLISH
   has been successfully sent. For QoS 0 this means the message has been completely
   written to the operating system. For QoS 1 this means we have received a PUBACK
   from the broker. For QoS 2 this means we have received a PUBCOMP form the broker */
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Message with the mid %d has been published.\n", mid);
}

/* Callback when disconnected */
void on_disconnect( struct mosquitto *mosq, void *obj, int rc )
{
	printf("Disconnected from broker: %s\n", mosquitto_strerror(rc));

	if( rc != 0 )
	{
		printf("Trying to reconnect...\n");
		mosquitto_reconnect_async(mosq);
	}
}
