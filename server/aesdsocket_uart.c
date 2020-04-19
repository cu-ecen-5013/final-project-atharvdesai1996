
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#                                       AESD ASSIGNMENT 6- aesdsocket.c
#                                   YOCTO ENVIRONMENT BRINGUP AND THREADING
#                                       AUTHOR: Aaksha Jaywant
#                                         IDENTIKEY: aaja1276
#			REFERENCE:  **https://beej.us/guide/bgnet/html/#a-simple-stream-server
#    		  			  http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
#			 			  Linux System Programming
#						  https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading
#						  https://github.com/stockrt/queue.h/blob/master/sample.c
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>
#include <syslog.h>
#include <pthread.h>
#include "queue.h"
#include <stdint.h>
#include<termios.h>   // using the termios.h library

#define BACKLOG 10 

#define USE_AESD_CHAR_DEVICE 0

uint8_t tswitchFLAG = 0;
FILE *file_ptr1, *file_ptr4;
//typedef struct slist_data_s slist_data_t;
//typedef struct thread_data_s thread_data_t;


/*struct slist_data_s 
{
	pthread_t thread_id;
	int new_fd_cp;
	int exit_flag;
	SLIST_ENTRY(slist_data_s) entries;
};

struct thread_data_s
{
	int new_fd_cp;
	int *exited_flag;
};

struct thread_data_s *ptr;*/

void uartty01_init(int file);
void uartty04_init(int file);

int flag_2=0, flag_3=0, sig_flag=0;

pthread_mutex_t resource_LOCK = PTHREAD_MUTEX_INITIALIZER;


/*************************************************************************************************************************************************/
static void signal_handler (int signo)
{
	sig_flag = 1;

	#ifndef USE_AESD_CHAR_DEVICE
	if(remove("/var/tmp/aesdsocketdata") == 0)
	{
		syslog(LOG_DEBUG, "%s\n", "FILE DELETED");
	}
	#endif
	
	if (signo == SIGINT)
		syslog(LOG_DEBUG, "%s\n", "Caught SIGINT!");
		
	else if (signo == SIGTERM)
		syslog(LOG_DEBUG, "%s\n", "Caught SIGTERM!");
	else
	{
		syslog(LOG_DEBUG, "%s\n", "Unexpected signal!");
		exit (EXIT_FAILURE);
	}
	//close(new_fd_s);
	exit (EXIT_SUCCESS);
}



/**************************************THREAD CREATION tty01 ****************************************************************************/

void *thread_tty01(void *arguments)
{
	printf("\nIn thread connection_handler\n");
	
	int *newSocket = ((int *)arguments);
	printf("newSocket %d",*newSocket);
	//char c; 
	uint8_t int_finFLAG = 0;
	char *ret_str;
	
	
	

	/*********Get line implementation *********************/
	char *line = NULL;
    size_t len = 0;
    size_t nread;

	//file_ptr = fopen("/home/aaksha/Desktop/aesdtest", "r");

    while ((nread = getline(&line, &len, file_ptr1)) != -1) 
    {
        printf("Retrieved line of length %zu:\n", nread);
        fwrite(line, nread, 1, stdout);
		ret_str = strstr(line,"Fingerprint matched");		//This fucnction compares the whole string with "FIngerprint matched"
        if(ret_str != NULL)								//data will be sent to the client only when "Fingerprint matched" string is received
        {
            printf("FOUND the string:::: %s",ret_str);
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line, nread, 0);
			pthread_mutex_unlock(&resource_LOCK);
			int_finFLAG = 1;
			tswitchFLAG = 1;
						
        }
		if(int_finFLAG == 1)
		{
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line, nread, 0);
			pthread_mutex_unlock(&resource_LOCK);
		}
    }
    free(line);
	tswitchFLAG = 0;
	printf("STAT tswitchFLAG ::::: %d",tswitchFLAG);
	printf("\nEXIT the connection handler\n");
	return NULL;
	
}

/**************************************THREAD CREATION tty01 *******************************************************************************/

void *thread_tty04(void *arguments)
{
	printf("\nIn thread connection_handler\n");
	int *newSocket = ((int *)arguments);
	//char c; 
	printf("newSocket %d",*newSocket);
	
		

		/*********Get line implementation *********************/
		char *line = NULL;
    	size_t len = 0;
    	size_t nread;


		while ((nread = getline(&line, &len, file_ptr4)) != -1) 
    	{
        	printf("Retrieved line of length %zu:\n", nread);
        	fwrite(line, nread, 1, stdout);
			if(tswitchFLAG == 1)
			{
				pthread_mutex_lock(&resource_LOCK);
				send(*newSocket, line, nread, 0);
				pthread_mutex_unlock(&resource_LOCK);
			}
		}


	free(line);
	printf("\nEXIT the connection handler\n");
	return NULL;
}




/************************************************************************************************************************************************/

int main(int argc, char *argv[]) //mainnnnn
{
	int fd1, fd4;

	file_ptr1 = fopen("/dev/ttyO1", "r");
	fd1 = fileno(file_ptr1);
	uartty01_init(fd1);

	file_ptr4 = fopen("/dev/ttyO4", "r");
	fd4 = fileno(file_ptr4);
	uartty01_init(fd4);
/**************************************************SIGNAL HANDLER *****************************************************************************/
	if (signal(SIGINT,signal_handler) == SIG_ERR)
	{
		//syslog(LOG_ERR, "%s\n", "Cannot handle SIGINT!");
		exit (EXIT_FAILURE);
	}

	if (signal (SIGTERM, signal_handler) == SIG_ERR)
	{
		//syslog(LOG_ERR, "%s\n", "Cannot handle SIGTERM!");
		exit (EXIT_FAILURE);
	}

/**************************************************************DAEMON*************************************************************************/

int flag=0;

	if (argc== 2)
	{
		int val = strcmp(argv[1],"-d");
		if(val == 0)
		{
			flag = 1;
		}
	}



/***********************************************************************************************************************************************/
int stat, fd_s, yes=1,new_fd_s;
struct addrinfo hints;
struct addrinfo *servinfo, *sock_ptr;
socklen_t addr_size;
struct sockaddr_storage their_addr;

/**********************************************************SOCKET INIT STEPS********************************************************************/

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;
hints.ai_protocol = 0;

	if ((stat = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0) //checking the status if getaddrinfo has returned a structure or not
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(stat));
		exit(1);
	}

	for (sock_ptr = servinfo; sock_ptr != NULL; (sock_ptr = sock_ptr->ai_next))
	{
		fd_s = socket(sock_ptr->ai_family, sock_ptr->ai_socktype,sock_ptr->ai_protocol); //setting socket parameters
		if (fd_s == -1)
            	continue;
		if (setsockopt(fd_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
            	exit(1);
		} 
		if (bind(fd_s, sock_ptr->ai_addr, sock_ptr->ai_addrlen) == -1)
		{
        		close(fd_s);
				continue;
		}    
		else
		{
			break;
		}            
	}

/**************************************************EXECUTE DAEMON IF FLAG SET **************************************************************/

	if(flag == 1)
	{
		pid_t pid;
		pid = fork();
		if(pid != 0)
		{
			exit(0);
		}
	}

	freeaddrinfo(servinfo); // free the linked-list

	if (sock_ptr == NULL)  
	{
		exit(1);
	}

	if (listen(fd_s, BACKLOG) == -1)
	{
		exit(1);
	}

	
/*************************************************************** ACCEPTING ************************************************************************/
//SLIST_INIT(&head);

//slist_data_t *threadParameter=NULL;

	while(sig_flag == 0)
	{
		
		printf("::::::::::::::accepting connection:::::::::::::::::::\n");

		addr_size = sizeof(their_addr);
		
		new_fd_s = accept(fd_s, (struct sockaddr *)&their_addr, &addr_size);
		
		if(new_fd_s == -1)
		{
			continue;
		}

		pthread_t t1, t2;


		pthread_create(&t1, NULL, thread_tty01, &new_fd_s);
		pthread_create(&t2, NULL, thread_tty04, &new_fd_s);

		
		
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
        
	}

		pthread_mutex_destroy(&resource_LOCK);
		fclose(file_ptr1);
		fclose(file_ptr4);
		printf("CAUGHT SOMETHING CYAA");

}
	
//Reference: Rucha Borwankar (writetobb.c)
//			https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c		

void uartty01_init(int file)
{
	printf("Entered program\n");

	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	printf("Initializing\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;	//control options
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options.c_oflag = 0;
	options.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately_TCSANOW
}	

//Reference: Rucha Borwankar (writetobb.c)
//			https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c					
void uartty04_init(int file)
{
	printf("Entered program\n");

	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	printf("Initializing\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;	//control options
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options.c_oflag = 0;
	options.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately_TCSANOW
}
	

