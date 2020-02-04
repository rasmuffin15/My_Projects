/*
Thomas Rasmussen
CSCI 4273 - Network Systems
Programing Assingment 2
*/

#ifndef __WEB_SERVER__
#define __WEB_SERVER__

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */


#define TRUE 1;
#define FALSE 0;
/* 
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure 
 */
int open_listenfd(int port);

// Handle HTTP GET requests.
void get_handler(char *requestbody, int clientConn);

// Handle HTTP POST requests.
void post_handler(char *requestbody, int clientConn);

#endif 