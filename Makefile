#Variable Defination
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g
LDFLAGS = -lmodbus -lcjson
OBJS = main.o modbus_tcp.o udp_server.o

#Target and its dependencies
HMI_Backend: $(OBJS)
	$(CC) $(CFLAGS) -o HMI_Backend $(OBJS) $(LDFLAGS)

#Compile .c to .o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

modbus_tcp.o: modbus_tcp.c
	$(CC) $(CFLAGS) -c modbus_tcp.c

udp_server.o: udp_server.c
	$(CC) $(CFLAGS) -c udp_server.c

#Clean build files
clean:
	@echo "cleaning object files and executable file...."
	@rm -f  *.o HMI_Backend
