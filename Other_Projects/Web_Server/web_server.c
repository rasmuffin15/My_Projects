/*
Thomas Rasmussen
CSCI 4273 - Network Systems
Programing Assingment 2
*/
#include "web_server.h"

//Global socket variables
int listenfd, *connfdp, port;
socklen_t clientlen = sizeof(struct sockaddr_in);
struct sockaddr_in clientaddr;


//Error function
void error(char *msg) {
  perror(msg);
  exit(1);
}

/* 
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure
 *
 * This code was taken from the third 'Helpful Links' in the 
 * assingment handout 
 */
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */


/*
 * This function gets the file extension of the
 * requested file from the client web browser
 *
 * Takes in the file and returns its extension
 */
char *getFileExtension(char *filePath) {
    
    const char ch = '.';
    char *ret = strrchr(filePath, ch);
    return ret;
}

/*
 * This function sets the content type for 
 * the response header
 *
 * Takes in a pointer to our content type variable 
 * and the file extension. Returns nothing. 
 */
void set_content_type(char *content_type, char *extension){

    // Default to unknown..
    sprintf(content_type, "text/unknown");
    
    if(strcmp(".js", extension) == 0)
        sprintf(content_type, "application/javascript");
     if(strcmp(".html", extension) == 0)
        sprintf(content_type, "text/html");
     if(strcmp(".png", extension) == 0)
        sprintf(content_type, "image/png");
     if(strcmp(".gif", extension) == 0)
        sprintf(content_type, "image/gif");
     if(strcmp(".jpg", extension) == 0)
        sprintf(content_type, "image/jpg");
     if(strcmp(".txt", extension) == 0)
        sprintf(content_type, "text/txt");
    if(strcmp(".pdf", extension) == 0)
        sprintf(content_type, "application/pdf");
    if(strcmp(".css", extension) == 0)
        sprintf(content_type, "text/css");
 }


/*
 * This function deals with a GET request. 
 *
 * Takes in the file name and the client socket
 */
void get_handler(char * buf, int clientConn) {

    if(strcmp(buf, "/favicon.ico") == 0)
        return;

    FILE * fp;
    char filePath[MAXLINE];
    char response[MAXLINE];
    char content_type[32];

    bzero(filePath, MAXLINE);

    //Every requested file will be in this directory
    strcpy(filePath, "www");
    strcat(filePath, buf);
    
    //Deal with request for '/'
    if(strcmp(filePath, "www/") == 0)
        strcpy(filePath, "www/index.html");

    //Get extension of requested file 
    char *extension = getFileExtension(filePath);
    bzero(content_type, 32);
    //Set the content type for HTTP response using extension
    set_content_type(content_type, extension);
    
    //Open requested file and send 404 response if error
    if((fp = fopen(filePath, "r")) == NULL) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
        write(clientConn, response, strlen(response));
        return;
    }
    

    // Get file size for Content-Length header
    fseek(fp, 0L, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    bzero(filePath, MAXLINE);
    
    //Write header back to client
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %lu\r\nContent-Type: %s\r\n\r\n", fileSize, content_type);
    write(clientConn, response, strlen(response));

    //Copy file content back to client one charactor at a time
    int c;

    while (!feof(fp)) {
        c = fgetc(fp);
        write(clientConn, &c, 1);
    }
    
}

//Given a bunch of request headers, return the value of the key
//Or NULL if it was not found.
char *parse_request_headers(char * buf, char *key)
{
    char* p = strtok(buf, "\n");

    while (p != NULL) {
        p = strtok(NULL, "\n");
        if (p != NULL) {
            if (strstr(p, key))
                return (p);
            
        }
    }

    return 0;

}

/*
 * This function handles the HTTP request.
 * Takes in client socket connection
 */
void http_protocol(int connfd) 
{
    //size_t n; 
    char buf[MAXLINE]; 
    char KeepAlive= FALSE;

    //Receive request from client web browser
    if(recv(connfd, buf, MAXLINE, 0) < 0)
        error("Failed in recv()"); 
    
    //Search HTTP Header for Connection type
    if (parse_request_headers(buf, "Connection: keep-alive"))
        KeepAlive = TRUE;
    

    // Parse request header for request type and file name
    char *temp = strtok(buf, " ");
    char *requestType = temp;
    temp = strtok(NULL, " ");
    char *fileName = temp;

    //Check what request type is. I thought I had to deal with
    // a POST request so that is why the second if statement is there
    if(strcmp(requestType, "GET") == 0) {
        get_handler(fileName, connfd);
        return;
    }
    else if(strcmp(requestType, "POST") == 0) {
        //post_handler(fileName, connfd);
        return;
    }

    // If we are here we got an invalid connection or one that we do not support.
    bzero(buf, MAXLINE);
    sprintf(buf, "HTTP/1.1 400 Bad Request\r\n\r\n");
    write(connfd, buf, strlen(buf));

    // If KeepAlive, set timer for 10 seconds.


}

/*
 * This function sets up a client connection and calls function
 * to deal with client HTTP request
 */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    free(vargp);
    http_protocol(connfd);
    close(connfd);
    return NULL;
}


int main(int argc, char **argv) 
{
    pthread_t tid;

    // We need to ignore SIGPIPE, in case the web browser kills the connection
    // before we are doing writing.

    sigaction(SIGPIPE, &(struct sigaction){{SIG_IGN}}, NULL);


    if (argc != 2) {
	   fprintf(stderr, "usage: %s <port>\n", argv[0]);
	   exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);

    
    while (1) {
	   connfdp = malloc(sizeof(int));
	   *connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
	   pthread_create(&tid, NULL, thread, connfdp);
    }

}