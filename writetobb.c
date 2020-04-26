/*******************************************************************************************
			Rucha Borwankar
	final aesd uart example code for receiving data from tiva
Simple send message example for communicating with the UART that is connected to a desktop PC. 
References:https://www.cmrr.umn.edu/~strupp/serial.html#2_5_4
https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c
Exploring Beaglebone black example code for transmitting to tiva board
https://class.ece.uw.edu/474/peckol/code/BeagleBone/ForestExamples/Example_6_UART/uart.c
Connections:UART 1 of Beaglebone		UART 3 of TIVA C series TM4C123G
		PIN 24-TxD	---------------->	PC-6-RxD
		PIN 26-RxD	---------------->       PC-7-TxD	
		  GND		---------------->         GND
***********************************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

int main()
{
	printf("Entered program\n");
	int file, count;
	//Open ttyO1-UART 1 file for receiving data 
	if ((file = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY))<0)
	{
	     perror("UART: Failed to open the file.\n");
	     return -1;
	}

   	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	printf("Initializing\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;	//control options
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options.c_oflag = 0;
	options.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately_TCSANOW
	
	//fcntl used to wait for read to occur
   	fcntl(file, F_SETFL, 0);
	
	printf("Waiting for read to happen!!!!\n");
	char receive[100];      //declare a buffer for receiving data
	   	if ((count = read(file, (void*) receive, 100)) < 0)
		{   
			//receive the data
		      	perror("Failed to read from the input\n");
		      	return -1;
		}
	
		//usleep(1000000);
	   	/*if (count==0) 
			printf("There was no data available to read!\n");
	   	else 
		{
		      printf("The following was read in [%d]: %s\n",count,receive);0
	   	}*/
		receive[count]='\0';
		printf("Accepted %d bytes,Message received=%d\n",count,receive);
		//printf("2nd element is number=%d,character= %c\n",receive[2],receive[2]);
	
	  	close(file);
	   	return 0;
}
