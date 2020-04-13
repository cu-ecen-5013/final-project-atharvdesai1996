ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#CFLAGS=-Wall -Werror 

all:
	$(CC) $(CFLAGS) $(INCLUDES) testbuild.c -o test 
	$(CC) $(CFLAGS) $(INCLUDES) writetobb.c -o writetobb
	$(CC) $(CFLAGS) $(INCLUDES) uartecho.c -o echopc
	$(CC) $(CFLAGS) $(INCLUDES) writetotiva.c -o writetotiva
clean:  
	rm -rf test
	rm -rf echopc
	rm -rf writetotiva
	rm -rf writetobb
