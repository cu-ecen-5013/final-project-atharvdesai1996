
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
#include <termios.h>   // using the termios.h library
#include <semaphore.h>
#define BACKLOG 10 

#define USE_AESD_CHAR_DEVICE 0

uint8_t tswitchFLAG = 0;
FILE *file_ptr1, *file_ptr4;
sem_t sem1, sem4;
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
	syslog(LOG_DEBUG, "In thread connection_handler of thread 11111\n");
	
	int *newSocket = ((int *)arguments);
	syslog(LOG_DEBUG, "newSocket %d",*newSocket);
	//uint8_t int_finFLAG = 0;
	char *ret_str;
	
	/*********Get line implementation *********************/
	char *line = NULL;
    size_t len = 0;
    size_t nread;

	//file_ptr = fopen("/home/aaksha/Desktop/aesdtest", "r");
//while(1)
//{
    while ((nread = getline(&line, &len, file_ptr1)) != -1) 
    {
		//sem_wait(&sem1);
       	syslog(LOG_DEBUG, "Retrieved line of length %zu:\n", nread);
        fwrite(line, nread, 1, stdout);
		ret_str = strstr(line,"Fingerprint matched");		//This function compares the whole string with "FIngerprint matched"
        if(ret_str != NULL)								//data will be sent to the client only when "Fingerprint matched" string is received
        {
           syslog(LOG_DEBUG, "FOUND the string:::: %s\n",ret_str);
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line, nread, 0);
			pthread_mutex_unlock(&resource_LOCK);
			syslog(LOG_DEBUG, "String Send\n");
			//int_finFLAG = 1;
			tswitchFLAG = 1;
			//return NULL;
						
        }
		/*if(int_finFLAG == 1)
		{
			syslog(LOG_DEBUG, "int_finFLAG == 1 in he ttyO1 thread\n");
			syslog(LOG_DEBUG, "::::::%s",line);
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line, nread, 0);
			pthread_mutex_unlock(&resource_LOCK);
			syslog(LOG_DEBUG, "temperature reading sent\n");
			int_finFLAG = 0;
		}*/
		
    }
    
	syslog(LOG_DEBUG, "STAT tswitchFLAG ::::: %d\n",tswitchFLAG);
	free(line);
	//if(nread == -1)
	//	break;
	
	
//}
	//tswitchFLAG = 0;
	//syslog(LOG_DEBUG, "STAT int_finFLAG  ::::: %d\n",int_finFLAG);
	
	syslog(LOG_DEBUG, "\nEXIT the connection handler\n");
	//sem_post(&sem4);
	return NULL;
	
}

/**************************************THREAD CREATION tty01 *******************************************************************************/

void *thread_tty04(void *arguments)
{
	syslog(LOG_DEBUG, "In thread connection_handler of thread 44444\n");
	int *newSocket = ((int *)arguments);
	//char c; 
	syslog(LOG_DEBUG, "newSocket %d",*newSocket);
			/*********Get line implementation *********************/
		char *line1 = NULL, *line4;
    	size_t len1 = 0, len4 =0;
    	size_t nread1,nread4;
	

	while(1)
	{
	if(tswitchFLAG == 1)
	{


		if ((nread1 = getline(&line1, &len1, file_ptr1)) != -1) 
    	{
			//sem_wait(&sem4);
        	syslog(LOG_DEBUG, "Retrieved line of length %zu:\n", nread1);
        	fwrite(line1, nread1, 1, stdout);
			syslog(LOG_DEBUG, "tswitchFLAG is 1 for thread file_ptr1\n");
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line1, nread1, 0);
			pthread_mutex_unlock(&resource_LOCK);
				//tswitchFLAG = 0;
		}

		if ((nread4 = getline(&line4, &len4, file_ptr4)) != -1) 
    	{
			syslog(LOG_DEBUG, "Retrieved line of length %zu:\n", nread4);
        	fwrite(line4, nread4, 1, stdout);
			syslog(LOG_DEBUG, "tswitchFLAG is 1 for thread O4\n");
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, line4, nread4, 0);
			pthread_mutex_unlock(&resource_LOCK);
				//tswitchFLAG = 0;
		
		
		}

	free(line1);
	free(line4);
	}
		if(nread1 == -1 || nread4 == -1)
		{
			tswitchFLAG = 0;
			break;	
		}

	}
	syslog(LOG_DEBUG, "\nEXIT the connection handler of tty04\n");
	//sem_post(&sem1);
	return NULL;
}




/************************************************************************************************************************************************/

int main(int argc, char *argv[]) //mainnnnn
{
	//int fd1, fd4;
	openlog ("UART_SOCKET", LOG_PERROR, LOG_USER);
	//file_ptr1 = fopen("/dev/ttyO1", "r");
	//fd1 = fileno(file_ptr1);
	//uartty01_init(fd1);
	file_ptr1 = fopen("/home/aaksha/Desktop/aesdtest", "r");

	//file_ptr4 = fopen("/dev/ttyO4", "r");
	//fd4 = fileno(file_ptr4);
	//uartty01_init(fd4);
	file_ptr4 = fopen("/home/aaksha/Desktop/aesdtest1", "r");

	/*if(sem_init(&sem1,0,0))
	{
		syslog(LOG_DEBUG,"FAILED to init sem1");
	}

	if(sem_init(&sem4,0,0))
	{
		syslog(LOG_DEBUG,"FAILED to init sem4");
	}*/

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
		
		syslog(LOG_DEBUG, "::::::::::::::accepting connection:::::::::::::::::::\n");

		addr_size = sizeof(their_addr);
		
		new_fd_s = accept(fd_s, (struct sockaddr *)&their_addr, &addr_size);
		
		if(new_fd_s == -1)
		{
			continue;
		}

		pthread_t t1, t2;


		pthread_create(&t1, NULL, thread_tty01, &new_fd_s);
		pthread_create(&t2, NULL, thread_tty04, &new_fd_s);

		//sem_post(&sem1);
		//sem_post(&sem4);
		
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
        
	}

		pthread_mutex_destroy(&resource_LOCK);
		closelog ();
		fclose(file_ptr1);
		fclose(file_ptr4);
		syslog(LOG_DEBUG, "CAUGHT SOMETHING CYAA");

}
	
//Reference: Rucha Borwankar (writetobb.c)
//			https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c		

void uartty01_init(int file)
{
	syslog(LOG_DEBUG, "Entered program\n");

	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	syslog(LOG_DEBUG, "Initializing\n");
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
	syslog(LOG_DEBUG, "Entered program\n");

	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	syslog(LOG_DEBUG, "Initializing\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;	//control options
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options.c_oflag = 0;
	options.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately_TCSANOW
}
	

