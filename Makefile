ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#ifeq ($(VPATH),)
#        VPATH= server
#endif

ifeq ($(LDFLAGS),)
        LDFLAGS= -pthread -lrt
endif

#CFLAGS=-Wall -Werror 

all:
	$(CC) $(CFLAGS) $(INCLUDES) testbuild.c -o test 
	$(CC) $(CFLAGS) $(INCLUDES) writetobb.c -o writetobb
	$(CC) $(CFLAGS) $(INCLUDES) uartecho.c -o echopc
	$(CC) $(CFLAGS) $(INCLUDES) writetotiva.c -o writetotiva
	$(CC) $(CFLAGS) $(INCLUDES) server/aesdsocket.c -o aesdsocket $(LDFLAGS)
	$(CC) $(CFLAGS) $(INCLUDES) client/socketclient.c -o socketclient
	$(CC) $(CFLAGS) $(INCLUDES) server/aesdsocket_uart.c -o aesdsocket_uart $(LDFLAGS)

clean:  
	rm -rf test
	rm -rf echopc
	rm -rf writetotiva
	rm -rf writetobb
	rm -rf aesdsocket
	rm -rf socketclient
