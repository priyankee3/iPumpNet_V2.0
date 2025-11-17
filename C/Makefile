#Variable Defination
CC = cc
CFLAGS = -Wall -g
LDFLAGS = -lmodbus -lcjson -lmosquitto
OBJS = main.o modbus_tcp.o mqtt_publish.o tcp_handle.c

#Target and its dependencies
HMI_Backend: $(OBJS)
	$(CC) $(CFLAGS) -o HMI_Backend $(OBJS) $(LDFLAGS)

#Compile .c to .o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

modbus_tcp.o: modbus_tcp.c
	$(CC) $(CFLAGS) -c modbus_tcp.c

mqtt_publish.o: mqtt_publish.c
	$(CC) $(CFLAGS) -c mqtt_publish.c

tcp_handle.0: tcp_handle.c
	$(CC) $(CFLAGS) -c tcp_handle.c

#Clean build files
clean:
	@echo "cleaning object files and executable file...."
	@rm -f  *.o HMI_Backend
