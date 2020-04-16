
#include "socketclient.h"


/* Function name : get_in_addr  
   Description   : gets sockaddr either for IPv4 or IPv6 */

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {                   //AF_INET: address family that is used to designate IPv4 addresses that your socket can communicate with
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);   // for IPv6
}


int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;

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

	
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {      // If recv function returns 0, indicates error
	    perror("recv");
	    exit(1);
	}
	//printf(" numbytes %d \n " ,numbytes);
	buf[numbytes] = '\0';

	printf("client: received  '%s' \n",buf);
	
	close(sockfd);

	return 0;
}


















