ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#CFLAGS=-Wall -Werror 

all: test

test: testbuild.c
	$(CC) $(CFLAGS) $(INCLUDES) testbuild.c -o test 
clean:  
	rm -rf test

