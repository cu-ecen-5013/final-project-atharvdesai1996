
#include "socketclient.h"


/* Function name : get_in_addr  
   Description   : gets sockaddr either for IPv4 or IPv6 */
static void signal_handler(int);
int sig_flag=0;
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {                   //AF_INET: address family that is used to designate IPv4 addresses that your socket can communicate with
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);   // for IPv6
}


int main(int argc, char *argv[])
{

///////////////////////////// Open Logging File  ///////////////////////////////////////////////////////////////
uint8_t i=0;
FILE *logfile;


/* char fingprt[] = "Fingerprint Logging";
char tempr[] = "Temperature Logging";  */

logfile  = fopen("/tmp/logfile", "a");


if (logfile == NULL)                   // to check if file has been created
{
   syslog(LOG_ERR, "%s", "ERROR : File path specified incorrectly"); // using log error to specify incorrect file path
   exit(1);                                  // exit code for error
}
else
{
   syslog(LOG_DEBUG,"%s","\n File opened");
}  


/////////////////////////////////////////SIGNAL HANDLER ////////////////////////////////////////////////////////
	if (signal(SIGINT,signal_handler) == SIG_ERR)
	{
		syslog(LOG_ERR, "%s\n", "Cannot handle SIGINT!");

		exit (EXIT_FAILURE);
	}

	if (signal (SIGTERM, signal_handler) == SIG_ERR)
	{
		syslog(LOG_ERR, "%s\n", "Cannot handle SIGTERM!");
		exit (EXIT_FAILURE);
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	time_t ltime; /* calendar time */
    	//ltime=time(NULL); /* get current cal time */
	struct tm * timeinfo;
	timeinfo = localtime (&ltime); 

// Addrinfo structure contains members such as ai_flags, ai_family, ai_socktype
	int rv;
	char s[INET6_ADDRSTRLEN];      // equals 46 i.e Length of the string form for IPv6.

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;      // UNSPEC returns socket addresses for any address family ( IPv4 or IPv6)
	hints.ai_socktype = SOCK_STREAM;  // Specifies either SOCK_STREAM or SOCK_DGRAM


/* Getaddrinfo has 4 arguments --> node , service, hints and res*/
/* 
       The getaddrinfo() function allocates and initializes a linked list of
       addrinfo structures, one for each network address that matches node
       and service, subject to any restrictions imposed by hints, and
       returns a pointer to the start of the list in res.  The items in the
       linked list are linked by the ai_next field. */

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
/* while(sig_flag==0)
{   */
 	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,        // creating sockfd from addrinfo parameters
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
/* The connect() system call connects the socket referred to by the file
       descriptor sockfd to the address specified by addr */


		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}
		
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

/* The inet_ntop function converts the network address structure src in the af
       address family into a character string.  The resulting string is
       copied to the buffer pointed to by dst, which must be a non-null
       pointer.  The caller specifies the number of bytes available in this
       buffer in the argument size.  */
	

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	while(sig_flag==0)
{  
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {      // If recv function returns 0, indicates error
	    perror("recv");
	    exit(1);
	}
	//printf(" numbytes %d \n " ,numbytes);
	buf[numbytes] = '\0';

/////////////////// Packet differentiation ///////////////////////////////////////////////////////////////////////

if (strchr(buf, 'F')!=NULL)
{
printf("Time:  %s",asctime( localtime(&ltime) ) );
fprintf(logfile, "%s \n",asctime (timeinfo));
//fwrite(fingprt, 1, sizeof(fingprt), logfile);
	for (i=0 ; i< 10; i++)
	{             
	printf(" Temperature Value :  '%d' \n",buf[i]);
	fprintf(logfile, "%d ",buf[i]);
	}
}

if (strchr(buf, 'U')!=NULL)
{
printf("Time:  %s",asctime( localtime(&ltime) ) ); 
//fwrite(tempr, 1, sizeof(tempr), logfile);  
	for (i=0 ; i< 10; i++)
	{             
	printf("Ultrasonic Data Value :  '%d' \n",buf[i]);
	fprintf(logfile, "%d ",buf[i]);
	}
}
////////////////////////////////////////////// Writing to file ///////////////////////////////////////////////////
	//fprintf(logfile, "%s", localtime(&ltime));
	//fwrite(buf, 1*(uint8_t), numbytes, logfile);
	
/*	printf("Time:  %s",asctime( localtime(&ltime) ) );   
	for (i=0 ; i< 10; i++)
	{             
	printf("Received :  '%d' \n",buf[i]);
	}     */
}
	close(sockfd);
	fclose(logfile);
	return 0;
}


//////////////////////////////////////////////////// Signal Handler /////////////////////////////////////////////////////////////////////
static void signal_handler (int signo)
{
	sig_flag = 1;

/*	#ifndef USE_AESD_CHAR_DEVICE
	if(remove("/var/tmp/aesdsocketdata") == 0)
	{
		syslog(LOG_DEBUG, "%s\n", "FILE DELETED");
	}
	#endif   */
	
	if (signo == SIGINT)
	{
		syslog(LOG_DEBUG, "%s\n", "Caught SIGINT!");
		printf("Caught SIGINT!");
	}
	else if (signo == SIGTERM)
	{
		syslog(LOG_DEBUG, "%s\n", "Caught SIGTERM!");
		printf("Caught SIGTERM!");
	}
	else
	{
		syslog(LOG_DEBUG, "%s\n", "Unexpected signal!");
		printf("Unexpected signal encountered");
		exit (EXIT_FAILURE);
	}
	//close(new_fd_s);
	exit (EXIT_SUCCESS);
}



