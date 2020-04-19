/*#################################################################################################################################################
#                                         AESD PROJECT                                                                                            #
# AUTHOR:  ATHARV SANDEEP DESAI                                                                                                                   #
# PROGRAM: socketclient                                                                                                                           #
# REFERENCES: 1)  https://www.gnu.org/software/libc/manual/html_node/Syslog-Example.html                                                          #
#             2)  https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c                                      #
#             3)  https://beej.us/guide/bgnet/html/#two-types-of-internet-sockets                                                                 #
#             4)  https://stackoverflow.com/questions/27525617/how-to-properly-compare-command-line-arguments                                     #
#             5)  https://raw.githubusercontent.com/freebsd/freebsd/stable/10/sys/sys/queue.h                                                     #
#             6)  https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading                                                    #
#	      7)  http://man7.org/linux/man-pages/man3/strchr.3.html                                                                              #
#             8)  https://stackoverflow.com/questions/9596945/how-to-get-appropriate-timestamp-in-c-for-logs                  		          #
#################################################################################################################################################*/


#ifndef SOCKETCLIENT_H_
#define SOCKETCLIENT_H_

/*##################################### Header Files ######################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>   //defines the sockaddr_in structure to store addresses for the Internet protocol family
#include <sys/socket.h>
#include <syslog.h>
#include <arpa/inet.h>    //makes available the type in_port_t in_addr structure
#include <time.h>
#include <signal.h>
#include <syslog.h>


/*##################################### Defining of Macros ######################################################*/

#define PORT "9000" // the client will be accessing the port number 9000 

#define MAXDATASIZE 500 // to assign a limit to max data read at once from the server 



#endif



