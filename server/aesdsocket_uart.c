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
#						  https://www.geeksforgeeks.org/ipc-using-message-queues/
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
#include <sys/ipc.h>
#include <sys/msg.h>
#define BACKLOG 10 

#define USE_AESD_CHAR_DEVICE 0

uint8_t tswitchFLAG = 0;
FILE *file_ptr1, *file_ptr4;
int fd1, fd4, fd1_copy;
sem_t sem1, sem4;

struct mesg_buffer 
{ 
    long mesg_type; 
    char mesg_text[200]; 
} message; 

int msgid; 

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
	int count1_copy;
	char* msg_q = malloc(200 * sizeof(char));
	fd1_copy = open("/dev/ttyO1", O_RDWR | O_CREAT | O_APPEND, 0664);
	//file_ptr1 = fopen("/dev/ttyO1", "r");
	/*********Get line implementation *********************/
	//char *line = NULL;
   // size_t len = 0;
    //size_t nread;

	//file_ptr = fopen("/home/aaksha/Desktop/aesdtest", "r");
//while(1)
//{
	//sem_wait(&sem1);
    //while ((nread = getline(&line, &len, file_ptr1)) != -1) 
	while((count1_copy = read(fd1_copy,msg_q,200*sizeof(char))) != 0)
    {
		//sem_wait(&sem1);
       	syslog(LOG_DEBUG, "Retrieved line of length %d:\n", count1_copy);
       // fwrite(line, nread, 1, stdout);
	   syslog(LOG_DEBUG, "DATA retrived isss ::::%s\n", msg_q);
		ret_str = strstr(msg_q,"yy");		//This function compares the whole string with "FIngerprint matched"
        if(ret_str != NULL)								//data will be sent to the client only when "Fingerprint matched" string is received
        {
           syslog(LOG_DEBUG, "FOUND the string:::: %s\n",ret_str);
			pthread_mutex_lock(&resource_LOCK);
			send(*newSocket, ret_str, 2*sizeof(char), 0);
			pthread_mutex_unlock(&resource_LOCK);
			syslog(LOG_DEBUG, "String Send\n");
			//int_finFLAG = 1;
			tswitchFLAG = 1;
			//sem_post(&sem4);
			syslog(LOG_DEBUG, "STAT tswitchFLAG ::::: %d\n",tswitchFLAG);
			free(msg_q);
			break;
			//
			//return NULL;
						
        }

		
    }
    

	syslog(LOG_DEBUG, "\nEXIT the connection handler\n");
	//fclose(file_ptr1);
	//close(fd1_copy);
	//sem_post(&sem4);
	return NULL;
	
}

/**************************************THREAD CREATION tty01 *******************************************************************************/

void *thread_tty04(void *arguments)
{
	syslog(LOG_DEBUG, "In thread connection_handler of thread with message queue*******\n");
	int *newSocket = ((int *)arguments);
	//char c; 
	syslog(LOG_DEBUG, "newSocket %d",*newSocket);
	char* msg_q1 = malloc(200 * sizeof(char));
	//char* msg_q2 = malloc(200 * sizeof(char));
			/*********Get line implementation *********************/
		//char *line1 = NULL, *line4;
    	//size_t len1 = 0, len4 =0;
    	//size_t nread1,nread4;
		//int count1, count4, id=1;
		int count1; //id=1;
		int num = 0; //i=0;
	syslog(LOG_DEBUG, "ABOVE WHILE 1 of thread_tty04\n");
	while(1)
	{
		syslog(LOG_DEBUG, "FLAG IN TTYO4 RXCD IS %d\n",tswitchFLAG);
	//sem_wait(&sem4);	
	if(tswitchFLAG == 1)
	{
		//tswitchFLAG = 0;
		syslog(LOG_DEBUG, "MESSAGEEEEE QUEUEEEEE LOOP ****\n");
		//count1 = read(fd1,message.mesg_text,200*sizeof(char));
		fcntl(fd1, F_SETFL, 0);
		//while(count1 = read(fd1,msg_q1,200*sizeof(char)) != 0)
		//{
			count1 = read(fd1,msg_q1,200*sizeof(char));
			syslog(LOG_DEBUG, "COUNT OF THE BYTES READ ARE 11111:::%d\n",count1);

		//syslog(LOG_DEBUG, "message queue is %s\n", message.mesg_text);
		/*if(count1 == 0)
		{
			syslog(LOG_DEBUG, "message queue didn't read anything from file tty01\n");
			break;
		}*/
		//message.mesg_type = id;
		
		
		//syslog(LOG_DEBUG, "NOT STUCK HEREEEEE\n");
		//count4 = read(fd4,&message.mesg_text[num+count1],sizeof(message.mesg_text));
		/*if(count4 == 0)
		{																										//uncomment for final testing
			syslog(LOG_DEBUG, "message queue didn't read anything from file tty04\n");
			break;
		}*/

		//message.mesg_type = id;
		//id += 1;
		
		//msgsnd(msgid, &message, sizeof(message), IPC_NOWAIT);
		syslog(LOG_DEBUG,"AFTER sending the MESSAGE %s \n", msg_q1);
		//msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT); 
		//send(*newSocket, message.mesg_text, (count1+count4), 0);
		//send(*newSocket, message.mesg_text, (count1), 0);
		send(*newSocket, msg_q1, (count1), 0);
		//}
		num += count1;
		//id += 1;
		
		/*for(i=0; i<201; i++)
			message.mesg_text[i] = 0;*/
		
	}

	}
	syslog(LOG_DEBUG, "\nEXIT the connection handler of tty04\n");
	close(fd1);
	close(fd4);
	free(msg_q1);
	tswitchFLAG = 0;
	//sem_post(&sem1);
	return NULL;
}




/************************************************************************************************************************************************/

int main(int argc, char *argv[]) //mainnnnn
{
	/*******Message Queue Implementation ********/
	/*key_t key; 
    
	 // ftok to generate unique key 
    key = ftok("progfile", 65); 
  
    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(key, 0666 | IPC_CREAT); */
   

	/***********************************************/
	
	openlog ("UART_SOCKET", LOG_PERROR, LOG_USER);

	fd1 = open("/dev/ttyO1", O_RDWR | O_CREAT | O_APPEND, 0664);
	if(fd1 < 0)
	{
		syslog(LOG_DEBUG, "ERRRRROOORRR opening file 1111111111111::: %d\n",fd1);
	}
	
	uartty01_init(fd1);

	fd4 = open("/dev/ttyO4", O_RDWR | O_CREAT | O_APPEND, 0664);
	if(fd4 < 0)
	{
		syslog(LOG_DEBUG, "ERRRRROOORRR opening file 4444444444:: %d\n",fd4);
	}
	syslog(LOG_DEBUG, "FDDDDD2 %d",fd4);


	if(sem_init(&sem1,0,0))
	{
		syslog(LOG_DEBUG,"FAILED to init sem1");
	}
	if(sem_init(&sem4,0,0))
	{
		syslog(LOG_DEBUG,"FAILED to init sem4");
	}
	//uartty04_init(fd4);

/**************************************************SIGNAL HANDLER *****************************************************************************/
	if (signal(SIGINT,signal_handler) == SIG_ERR)
	{
		
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

	while(sig_flag == 0)
	{
		syslog(LOG_DEBUG, "::::::::::::::accepting connection:::::::::::::::::::\n");

		addr_size = sizeof(their_addr);
		syslog(LOG_DEBUG, "STUCK above accept\n");
		new_fd_s = accept(fd_s, (struct sockaddr *)&their_addr, &addr_size);
		syslog(LOG_DEBUG, "STUCK below accept\n");
		if(new_fd_s == -1)
		{
			syslog(LOG_DEBUG, "NOT CONNECTED TO THE CLIENT\n");
			continue;
		}
		syslog(LOG_DEBUG, "ACCEPTED CONNECTION FROM CLIENT\n");
		pthread_t t1, t2;

		syslog(LOG_DEBUG, "CREATING THREADS\n");
		pthread_create(&t1, NULL, thread_tty01, &new_fd_s);
		//sleep(1);
		pthread_create(&t2, NULL, thread_tty04, &new_fd_s);

		//sem_post(&sem1);
		//sem_post(&sem4);
		
		pthread_join(t1,NULL);
		pthread_join(t2,NULL);
        
	}

		pthread_mutex_destroy(&resource_LOCK);
		closelog ();
		//fclose(file_ptr1);
		//fclose(file_ptr4);
		syslog(LOG_DEBUG, "CAUGHT SOMETHING CYAA");

}
	
//Reference: Rucha Borwankar (writetobb.c)
//			https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c		

void uartty01_init(int file)
{
	syslog(LOG_DEBUG, "Entered init O1\n");

	struct termios options1;               //The termios structure is vital
   	tcgetattr(file, &options1);            //Sets the parameters associated with file
	syslog(LOG_DEBUG, "Initializing O1\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options1.c_cflag = B9600 | CS8 | CREAD | CLOCAL;	//control options
  	options1.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options1.c_oflag = 0;
	options1.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options1);  //changes occur immmediately_TCSANOW
}	

//Reference: Rucha Borwankar (writetobb.c)
//			https://github.com/derekmolloy/exploringBB/blob/version2/chp08/uart/uartEchoC/BBBEcho.c					
void uartty04_init(int file)
{
	syslog(LOG_DEBUG, "Entered program O4\n");

	struct termios options4;               //The termios structure is vital
   	tcgetattr(file, &options4);            //Sets the parameters associated with file
	syslog(LOG_DEBUG, "Initializing O4\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options4.c_cflag = B9600 | CS8 | CREAD | CLOCAL;	//control options
  	options4.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options4.c_oflag = 0;
	options4.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options4);  //changes occur immmediately_TCSANOW
}
