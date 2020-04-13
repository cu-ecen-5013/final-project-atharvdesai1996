/*******************************************************************************************
			Rucha Borwankar
	final aesd uart example code for testing transmission to tiva board
Simple send message example for communicating with the UART that is connected to a desktop PC. 
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

	usleep(1000000);                  //wait for tiva to respond 
	
	//fcntl used to wait for read to occur
   	fcntl(file, F_SETFL, 0);
	printf("fcntl set \n");
	printf("Waiting!!");
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
