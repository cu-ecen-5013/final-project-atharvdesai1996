
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

#define BACKLOG 10 

#define USE_AESD_CHAR_DEVICE 1

typedef struct slist_data_s slist_data_t;
//typedef struct thread_data_s thread_data_t;


struct slist_data_s 
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

struct thread_data_s *ptr;


int flag_2=0, flag_3=0, sig_flag=0;
FILE *file_ptr;
pthread_mutex_t resource_LOCK = PTHREAD_MUTEX_INITIALIZER;
//,*pvar=NULL;
SLIST_HEAD(slisthead, slist_data_s) head;

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




/************************************** THREAD CREATION ******************************************************************************************/

void *connection_handler(void *arguments)
{
	printf("\nIn thread connection_handler\n");
	struct thread_data_s *args = (struct thread_data_s *)arguments;
	char *rec;
	int *exit_flag = args->exited_flag;
	printf(" \n Inside my thread func \n");
	int mall_COUNT = 0, temp=0, rec_stat=0;
	char c, *ret_str=NULL;
	//int newSocket = *((int *)arg);
	rec = (char *)malloc(1024 * sizeof(char));
	while(1)						
	{
		
		
		rec_stat = recv(args->new_fd_cp, (rec+temp), (1024 * sizeof(char)), 0);
		if(rec_stat == -1)
		{
			printf("NOTHING WAS RECEIVED\n");
		}
		printf("REC_STAT:::::: %d",rec_stat);
		if(rec_stat > 0)
		{
			printf("writingggg:::::\n");

			#if USE_AESD_CHAR_DEVICE
				file_ptr = fopen("/dev/aesdchar", "a");
			#else 
				file_ptr = fopen("/var/tmp/aesdsocketdata", "a");
			#endif

			pthread_mutex_lock(&resource_LOCK); //
			fwrite((rec+temp), 1, rec_stat,file_ptr);
			pthread_mutex_unlock(&resource_LOCK); 
			fclose(file_ptr);
			
			#if USE_AESD_CHAR_DEVICE
				file_ptr = fopen("/dev/aesdchar", "r");
			#else 
				file_ptr = fopen("/var/tmp/aesdsocketdata", "r");
			#endif

			while(1)
			{
				c = fgetc(file_ptr);
				if (feof(file_ptr))
				{
					break;
				}
				send(args->new_fd_cp, &c, 1, 0);
			}
			fclose(file_ptr);
			temp += rec_stat;
				ret_str = strchr((rec+temp), '\n');
				if (ret_str == NULL)
				{
					rec = realloc(rec,(temp+(rec_stat)));
					mall_COUNT++;
				}
		}
		else if(rec_stat == 0)
		{
			
			free(rec);	
			//close(args->new_fd_cp);
			break;
		}
	
	}
	
	//printf("args->exit::: %d\n",args->exited_flag);

	printf("\nEXIT the connection handler\n");
	*exit_flag = 1;
		free(args);
	
	return NULL;
	
}


/************************************************************************************************************************************************/

int main(int argc, char *argv[]) //mainnnnn
{

/**************************************************	SIGNAL HANDLER *****************************************************************************/
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

/************************************************************** DAEMON *************************************************************************/

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
SLIST_INIT(&head);

slist_data_t *threadParameter=NULL;

	while(sig_flag == 0)
	{
		
		printf("::::::::::::::accepting connection:::::::::::::::::::\n");

		addr_size = sizeof(their_addr);
		
		new_fd_s = accept(fd_s, (struct sockaddr *)&their_addr, &addr_size);
		
		if(new_fd_s == -1)
		{
			continue;
		}
		//sleep(1);
		threadParameter = (slist_data_t*)malloc(sizeof(slist_data_t));
		if (threadParameter == NULL)
        {
            exit(EXIT_FAILURE);
        }
	
		ptr = (struct thread_data_s*)malloc(1*sizeof(struct thread_data_s));
		ptr->new_fd_cp = new_fd_s;
		ptr->exited_flag = &(threadParameter->exit_flag);
		//threadParameter->new_fd_cp = new_fd_s;
		pthread_t t;
		pthread_create(&t, NULL, connection_handler, (void *)ptr);
		syslog(LOG_DEBUG,"%s","Thread created");
			
		SLIST_INSERT_HEAD(&head, threadParameter, entries);
		threadParameter->thread_id = t;
		threadParameter = NULL;
		slist_data_t *last = NULL;  

		SLIST_FOREACH_SAFE(threadParameter,&head,entries, last)
		{
			if (threadParameter->exit_flag == 1)
			{	
        		printf("exit flag I'M SAFE\n");
				pthread_join(threadParameter->thread_id,NULL);
				SLIST_REMOVE(&head, threadParameter, slist_data_s, entries);	
				free(threadParameter);
			}
		}


		while (!SLIST_EMPTY(&head))
		{
		slist_data_t *last = NULL;
        SLIST_FOREACH_SAFE(threadParameter, &head, entries, last)
        {
            	if(threadParameter->exit_flag == 1)
            	{
					//printf("gracefulllllll::::\n");
                	pthread_join(threadParameter->thread_id, NULL);
                	SLIST_REMOVE(&head, threadParameter, slist_data_s, entries);
                	free(threadParameter);
            	}
        }
		}
		

			
        
	}
	pthread_mutex_destroy(&resource_LOCK);
	printf("CAUGHT SOMETHING CYAA");

}
	
	
			

	

