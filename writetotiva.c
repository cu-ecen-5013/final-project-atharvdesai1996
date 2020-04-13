/*******************************************************************************************
			Rucha Borwankar
	final aesd uart example code for testing transmission to tiva board
References:https://www.cmrr.umn.edu/~strupp/serial.html#2_5_4 
https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c
Exploring Beaglebone black example code for transmitting to tiva board
***********************************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

int main(){
	int file, count;

	if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY))<0)
	{
	     perror("UART: Failed to open the file.\n");
	     return -1;
	}
   	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file

   	// Set up the communications options:
   	//   9600 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

	unsigned char transmit[18] = " Hello BeagleBone!";  //the string to send

   	if ((count = write(file, &transmit,18))<0)
	{
	        //send the string
      		perror("Failed to write to the output\n");
      		return -1;
   	}
	printf("Write successful\n");
   	
	usleep(1000000);    
	close(file);
	return 0;            
}
