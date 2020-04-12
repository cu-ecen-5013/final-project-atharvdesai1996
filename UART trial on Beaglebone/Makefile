ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#CFLAGS=-Wall -Werror 

all:
	$(CC) $(CFLAGS) $(INCLUDES) uarttransrecv.c -o uartrecv
	$(CC) $(CFLAGS) $(INCLUDES) uartechocomp.c -o echopc
clean:  
	rm -rf echopc
	rm -rf uartrecv
