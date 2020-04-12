ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#CFLAGS=-Wall -Werror 

all:
	$(CC) $(CFLAGS) $(INCLUDES) testbuild.c -o test 
	$(CC) $(CFLAGS) $(INCLUDES) uartbbtiva.c -o tivabb
	$(CC) $(CFLAGS) $(INCLUDES) uartecho.c -o echopc
clean:  
	rm -rf test
	rm -rf echopc
	rm -rf tivabb