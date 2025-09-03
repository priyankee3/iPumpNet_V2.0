#include"header.h"
void mqtt_publish(s8 *ip)
{
	struct mosquitto *mosq;
	s32 rc;
	s8* message = NULL;

	/* Create a new client instance
	   id = NULL -> ask the broker to generate a client id for us
	   clean session = true -> the broker should remove old sessions when we connect
	   obj = NULL -> we aren't passing any of our private data for call back */

	mosq = mosquitto_new(NULL, true, NULL);
	if( mosq == NULL )
	{
		fprintf(stderr,"Error: Out of memory.\n");
	}
	
	// Configure callback. This should be done before connecting ideally
	mosquitto_connect_callback_set(mosq,on_connect);
	mosquitto_publish_callback_set(mosq,on_publish);
	
	/* Connect to Broker on port 1883, with a keepalive of 60 seconds.
	   This call make the socket connection only, it does not complete
	   the MQTT CONNECT/CONNACK flow, you should use mosquitto_loop_start()
	   or mosquitto_loop_forever() for processing net traffic */
	rc = mosquitto_connect(mosq, ip, 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS)
	{
		mosquitto_destroy(mosq);
		fprintf(stderr,"Error: %s\n", mosquitto_strerror(rc));
	}

	// Runs network loop in a background thread, this calls returns quicky
	rc = mosquitto_loop_start(mosq);
	if(rc != MOSQ_ERR_SUCCESS)
	{
		mosquitto_destroy(mosq);
		fprintf(stderr,"Error: %s\n", mosquitto_strerror(rc));
	}
	
	/* Publish Message 
	   mosq - our client instance
	   mid = NULL - we don't want to know what the message id for this message is 
	   topic = "test/topic" - the topic on which this message will be publish
	   payload - the actual payload
	   qos = 2 - publish with QoS for this example 
	   retain = false - do not use the retained message feature for this message */
	
	message = cJSON_Print(json_send);

	rc = mosquitto_publish(mosq,NULL, "THD/Data", strlen(message), message, 0, false );
	if(rc != MOSQ_ERR_SUCCESS)
	{
		mosquitto_destroy(mosq);
		fprintf(stderr,"Error: %s\n", mosquitto_strerror(rc));
	}
	
	mosquitto_disconnect(mosq);
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
