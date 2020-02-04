//
// Created by Thomas Rasmussen on 12/10/19.
//

#include "dfHeader.h"

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

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
}

int validateUser(char* username, char* password) {

    FILE *fp;
    char temp[120];
    char line[120];

    sprintf(temp, "%s %s", username, password);

    if((fp = fopen("dfs.conf", "r")) == NULL) {
        perror("Could not open dfs.conf");
        exit(-1);
    }

    while(fgets(line, 120, fp) != NULL) {
        line[strlen(line) -1] = '\0';
        if (strcmp(temp, line) == 0)
            return (1);
   }
    fclose(fp);
    return (0);
}

void handlePut(char *buf, int connfd, size_t bufsize, char *username, char *args) {
    size_t buf_offset = 0;

    // The first two lines in buf is not useful, so we need to find the offset to the real data. (line 3+)
    // We do this by searching for the '\n' character, when we have found two of them we are good.
    int nn = 0;
    for (size_t i = 0; i < bufsize; i++) {
        if (buf[i] == '\n')
            nn++;
        if (nn == 2) {
            buf_offset = i + 1;
            break;
        }
    }

    char filepath[120];
    size_t nbytes;
    sprintf(filepath, "%s/%s", working_dir, username);
    mkdir(filepath, S_IRUSR | S_IWUSR | S_IXUSR);
    bzero(filepath, 120);
    sprintf(filepath, "%s/%s/%s", working_dir, username, args);
    FILE *fp_fragment = fopen(filepath, "w");
    // If there was left over in the buffer then we write that out first and continue reading
    // from the socket till all data has been received.
    if (buf_offset < bufsize) {
        fwrite(buf + buf_offset, 1, bufsize - buf_offset, fp_fragment);
    }
    while ((nbytes = recv(connfd, buf, MAXBUF, 0)) > 0) {
        fwrite(buf, 1, nbytes, fp_fragment);
    }
    fclose(fp_fragment);
}

void handleGet(int connfd, char *username, char *args){
    // args is the filename of the file we should retrieve.
    // Scan the directory and see if we have a match.
    char dirpath[120];
    struct dirent *de;
    char *chunknames[2];
    size_t chunksizes[2], nbytes;
    int index = 0;
    char filename[120], completefilepath[120];
    struct stat fs;
    char buf[MAXBUF];
    FILE *fp;


    fprintf(stderr,"--> handleGet(%s)", args);
    sprintf(dirpath, "%s/%s", working_dir, username);
    DIR *dr = opendir(dirpath);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return;
    }
    sprintf(filename,".%s.", args );

    while ((de = readdir(dr)) != NULL) {
        printf("%s\n", de->d_name);
        if (startsWith(filename, de->d_name)) {
            printf("Found a match\n");
            sprintf(completefilepath,"%s/%s/%s", working_dir, username, de->d_name);
            stat(completefilepath, &fs);
            chunknames[index] = strdup(de->d_name);
            chunksizes[index++] = fs.st_size;
            if (index == 2)
                break;
        }
    } // while loop.
    closedir(dr);

    if (index != 2) { // We didn't match anything.
        fprintf(stderr, "getHandler(%s) was not able to find fragments\n", args);
        // Send error message back.
        sprintf(buf, "ERR: File Not found.\r\n");
        write(connfd, buf, strlen(buf));
        // close(connfd);
    } else {
        sprintf(buf,"%s %zu %s %zu\r\n", chunknames[0], chunksizes[0], chunknames[1], chunksizes[1]);
        write(connfd, buf, strlen(buf));
        for (int i =0; i < 2; i++) {
            sprintf(completefilepath,"%s/%s/%s", working_dir, username, chunknames[i]);

            if ((fp = fopen(completefilepath, "r")) == NULL){
                perror("Failed to open file.");
                return;
            }

            while ((nbytes = fread(buf, 1, MAXBUF, fp)) > 0) {
                printf("Read %zu bytes", nbytes);
                write(connfd, buf, nbytes);
            } // while
            fclose(fp);
        } // for
    } // else

}

void handleList(int connfd, char *username){

    char dirpath[120];
    struct dirent *de;
    char fileentry[MAXLINE];

    sprintf(dirpath, "%s/%s", working_dir, username);
    DIR *dr = opendir(dirpath);

    while ((de = readdir(dr)) != NULL) {
        // Filter out the .. and . files.
        if (strcmp(de->d_name, "..") == 0)
            continue;
        if (strcmp(de->d_name, ".") == 0)
            continue;
        bzero(fileentry, MAXLINE);
        sprintf(fileentry,"%s\r\n", de->d_name);

        write(connfd, fileentry, strlen(fileentry));
    }
    close(connfd);
}

void requestHandler(int connfd) {
    /*
     * Expected format is:
     * Line 1: username password
     * Line 2: command {args}
     * Line 3+: data
     */
    char buf[MAXBUF];
    char *username;
    char *password;
    char *command;
    char *args;

    size_t ret = recv(connfd, buf, MAXBUF, 0);
    // fprintf(stderr, "%s/ read %zu bytes\n", working_dir, ret);

    char *tmp = strdup(buf);
    char *tmp1 = strtok(tmp, "\r");
    username = strdup(strtok(tmp1," "));
    password = strdup(strtok(NULL,""));
    free(tmp);

    // fprintf(stderr, "%s/%s/%s\n", working_dir, username, password);

    char *tmp_cmd = strdup(buf);
    char *tmp_arg = strtok(tmp_cmd, "\n");
    tmp_arg = strtok(NULL, "\n");
    command = strdup(strtok(tmp_arg," "));

    // Handle the case where there are not args to the command. Only the 'list' command.
    if (strcmp(command, "list\r")) {
        args = strdup(strtok(NULL,""));
        args[strlen(args) -1] = '\0'; }
    else {
        command[strlen(command) - 1] = '\0';
    }
    free(tmp_cmd);

    if(!validateUser(username, password)) {
        fprintf(stderr, "Validation Failed\n");
        return;
    }

    if(!strncmp(command, "list", 4)) {
        handleList(connfd, username);
    }
    else if(!strncmp(command, "get", 3)) {
        handleGet(connfd, username, args);

    }
    else if(!strncmp(command, "put", 3)) {
        // Need to pass on the buf, since there could be real
        // data that needs to be saved.
        handlePut(buf, connfd, ret, username, args);
    }
    else if(!strncmp(command, "exit", 4)) {
        printf("running exit() on server\n");

    }

}

void * thread(void * vargp)
{
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);

    if(fork() == 0) {
        requestHandler(connfd);
        close(connfd);
        exit(0);
    } else {
        signal(SIGCHLD,SIG_IGN);
    }
    return NULL;
}

int main(int argc, char ** argv) {

    char buf[MAXBUF]; /* message buf */
    pthread_t tid;
    int portno;

    if(argc != 3) {
        fprintf(stderr, "Arguments incorrect");
        exit(-1);
    }

    // We work from our CWD.
    sprintf(working_dir,"./%s", argv[1]);
    // Just create and ignore if its already there.
    mkdir(working_dir, S_IRUSR|S_IWUSR|S_IXUSR); // Owner Read/Write
    portno = atoi(argv[2]);

    //chdir(working_dir);
    //Creates TCP socket connection
    listenfd = open_listenfd(portno);

    while (1) {
        int *connfdp = malloc(sizeof(int));
        *connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
        if (connfdp < 0 ) {
            perror("accept() failed.");
            exit(-1);
        }
        pthread_create(&tid, NULL, thread, connfdp);
    }
}