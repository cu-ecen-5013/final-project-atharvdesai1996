/*******************************************************************************************
			Rucha Borwankar
			final aesd uart example code for testing echo
Simple send message example for communicating with the UART that is connected to a desktop PC.
References:-
https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c
Exploring Beaglebone black example code for echo to computer
https://www.cmrr.umn.edu/~strupp/serial.html#2_5_4
***********************************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int main(int argc, char *argv[]){
   int file, count;
   if(argc!=2){
       printf("Please pass a message string to send, exiting!\n");
       return -2;
   }
   if ((file = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the device.\n");
      return -1;
   }
   struct termios options;
   tcgetattr(file, &options);
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);

   // send the string plus the null character
   if ((count = write(file, argv[1], strlen(argv[1])+1))<0){
      perror("UART: Failed to write to the output.\n");
      return -1;
   }
   close(file);
   printf("Finished sending the message, exiting.\n");

   if ((file = open("/dev/ttyO0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the device.\n");
      return -1;
   }
   fcntl(file, F_SETFL, 0);
	printf("fcntl set \n");

	unsigned char receive[100];      //declare a buffer for receiving data
   	if ((count = read(file, (void*)receive, 100))<0)
	{   
		//receive the data
	      	perror("Failed to read from the input\n");
	      	return -1;
	}

   	if (count==0) printf("There was no data available to read!\n");
   	else 
	{
	      printf("The following was read in [%d]: %s\n",count,receive);
   	}
  	close(file);
   return 0;
}
