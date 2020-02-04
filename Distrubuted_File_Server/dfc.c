//
// Created by Thomas Rasmussen on 11/29/19.
//

#include <fcntl.h>
#include <sys/errno.h>
#include "dfHeader.h"

// Our config data structure, represents the config file info.
struct dfsconfig config;


// CLion makes this looks like a mess, but it is correct.
static struct distribution dist_table[4] = {
        {
                {
                    {1,2}, {2, 3}, {3, 4}, {4, 1}}
                },
        {
                {{4, 1}, {1, 2}, {2, 3}, {3 ,4}}
                },
        {
                {{3, 4}, {4, 1}, {1, 2}, {2, 3}}
        },
        {
                {{2, 3}, {3, 4}, {4, 1}, {1, 2}}
        }
};

void init_config(char *filename) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        perror("Not able to open config file");
        exit(-1);
    }

    char throwaway[10];
    fscanf(fp, "%s %s %[^:]:%d", throwaway, throwaway, config.servers[0].hostname, &config.servers[0].port);
    fscanf(fp, "%s %s %[^:]:%d", throwaway, throwaway, config.servers[1].hostname, &config.servers[1].port);
    fscanf(fp, "%s %s %[^:]:%d", throwaway, throwaway, config.servers[2].hostname, &config.servers[2].port);
    fscanf(fp, "%s %s %[^:]:%d", throwaway, throwaway, config.servers[3].hostname, &config.servers[3].port);
    fscanf(fp, "%[^:]:%s", throwaway, config.username);
    fscanf(fp, "%[^:]:%s", throwaway, config.password);
}

//Error function
void error(char *msg) {
    perror(msg);
    exit(1);
}

int md5_checksum(FILE *fp) {
    unsigned char digest[16];
    char buf[MAXBUF];
    long sum;
    size_t nbytes;
    CC_MD5_CTX context;
    CC_MD5_Init(&context);
    while ((nbytes = fread(buf, 1, MAXBUF, fp)) > 0) {
        CC_MD5_Update(&context, buf, nbytes);
    }

    CC_MD5_Final(digest, &context);

    // We do a cross sum of the digest for the result.
    for(int i = 0; i < 16; ++i)
        sum += (unsigned int)digest[i];

    rewind(fp);
    return(sum % 4);
}



void listFiles() {

     int serverConn;
     char buf[MAXBUF];
     char filelist[MAXBUF * 4];
     long offset = 0;
     char server_failure = FALSE;
     filenode *rootnode = NULL;

    // We need to go to all servers and get a list of files available.
    // Then parse it all together to see if we have all parts of all files.
    for (int i = 0; i < 4; i++ ) { // 4 Servers
        serverConn = establishConnection(i);
        if (serverConn == 0) {
            server_failure = TRUE;
            continue; // Skip any server we can't talk to.
        }
        struct timeval timeout = { 0, 5000};
        int r = setsockopt(serverConn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        // User/Password
        bzero(buf,MAXBUF);
        sprintf(buf, "%s %s\r\n", config.username, config.password);
        write(serverConn, buf, strlen(buf));

        // Command next.
        bzero(buf,MAXBUF);
        sprintf(buf, "list\r\n");
        write(serverConn, buf, strlen(buf));

        // Now read back a list of available files.
        long nbytes;
        bzero(buf, MAXBUF);
        while (( nbytes = recv(serverConn, buf, MAXBUF, 0))  > 0) {
            if (nbytes + offset > MAXBUF * 4) {
                printf("Error, could not list all files, due to buffer overflow.\n");
                return;
            }
           bcopy(buf, filelist + offset, nbytes);
           offset += nbytes;
        }
        close(serverConn);
    }

    // Parse the buffer.
    char *filename = strtok(filelist, "\r\n");
    char *tmp;
    char frag_n[2];
    while ((tmp = strtok(NULL,"\r\n")) != NULL) {
        char tmp_name[120];
        memcpy(tmp_name, tmp+1, strlen(tmp) - 3);
        frag_n[0] = tmp[strlen(tmp) - 1];
        frag_n[1] ='\0';
        int fragment = atoi(frag_n);
        tmp_name[strlen(tmp) - 3] = '\0';
        // Search to see if we have this one already.
        filenode *entry = fn_findbyname(&rootnode, tmp_name);
        if (entry == NULL) {
            entry = fn_new_filenode(tmp_name);
            fn_push(entry, &rootnode);
        }
        // Update the fragment
        fn_setframent(entry, fragment);
    }

    fn_printall(&rootnode);
    fn_cleanup(&rootnode);
}

void requestFile(int serverConn, char *filename) {
    char buf[MAXBUF];
    char metadata[MAXLINE];
    size_t nbytes;
    char *chunknames[2], *token;
    int chunksize[2];
    size_t maxread;

    bzero(buf, MAXBUF);
    sprintf(buf, "%s %s\r\n", config.username, config.password);
    write(serverConn, buf, strlen(buf));

    bzero(buf, MAXBUF);
    sprintf(buf, "get %s\r\n", filename);
    write(serverConn, buf, strlen(buf));

    // Expected format coming back is one line with metadata. Always 2 chunks on a server.
    // Line1 : filename E.g. <filename.extension.chunknumber> <size of chunk> <filename.extension.chunknumber> <size..>
    // Line2+ : data of chunk1 and 2.

    fgets(metadata, MAXLINE, fdopen(serverConn, "r"));
    if (strcmp(metadata, "ERR: File Not found.\r\n") == 0) {
        printf("%s",metadata);
        return;
    }

    char *tmp = strdup(metadata);
    char *tmp1 = strtok(tmp, "\r");
    chunknames[0] = strdup(strtok(tmp1," "));
    chunksize[0] = atoi(strtok(NULL," "));
    chunknames[1] = strdup(strtok(NULL," "));
    chunksize[1] = atoi(strtok(NULL, ""));
    free(tmp);


    for (int i = 0; i < 2; i++) {
        FILE *fp = fopen(chunknames[i], "w");
        if (chunksize[i] > MAXBUF) {
            maxread = MAXBUF;
        } else {
            maxread = chunksize[i];
        }
        while ((nbytes = recv(serverConn, buf, maxread, 0)) > 0) {
            fwrite(buf, 1, nbytes, fp);
            chunksize[i] -= nbytes;
            if (chunksize[i] == 0) {
                fclose(fp);
                break;
            }
        } // While loop
    } // For loop
}

void getFile(char *filename) {
    int serverConn;
    char server_failure = FALSE;

    // We only have to contact 2 servers to get a particular file, either 0,2 or 1,3
    int s[] = {0,2,  1, 3};

    // Need to get rid of the '\n'
    filename[strlen(filename) -1 ] = '\0';

    for (int i = 0 ; i < 2; i++) {
        serverConn = establishConnection(s[i]);
        if (serverConn == 0) {
            server_failure = TRUE;
            break; // Either server 0 or/and 2 is down. Lets try 1 and 3.
        }
        requestFile(serverConn, filename);
        close(serverConn);
    }
    if (server_failure) { // Try server 1 and 3
        printf("Failure on either server 0 or 2 or both, trying 1 and 3\n");
        server_failure = FALSE;
        for (int i = 2 ; i < 4; i++) {
            serverConn = establishConnection(s[i]);
            if (serverConn == 0) {
                server_failure = TRUE;
                break; // Failure on 1 or 3
            }
            requestFile(serverConn, filename);
            close(serverConn);
        }
    }
    if (server_failure) {
        printf("Too many servers are down, not able to get file %s\n", filename);
        return;
    }

    // At this point we should have all 4 chunks. We need to put them together.
    FILE *fp_out = fopen(filename, "w");
    char file_in_name[80];
    size_t nbytes;
    char buf[MAXBUF];

    for (int i = 1; i < 5; i ++) {
        sprintf(file_in_name, ".%s.%i", filename, i);
        FILE *fp_in = fopen(file_in_name, "r");
        while ((nbytes = fread(buf, 1, MAXBUF, fp_in)) > 0) {
            // TODO: Decrypt content here..
            fwrite(buf, 1, nbytes, fp_out);
        }
        fclose(fp_in);
        unlink(file_in_name); // Delete chunk fragment.
    }
    fclose(fp_out);
    //printf("Received file %s\n", filename);

}


int establishConnection(int server_entry) {
    struct addrinfo hints, *res;
    long sock_arg;
    fd_set myset;
    struct timeval tv;
    int valopt, result;
    socklen_t lon;
    char str_port[12];

    // Lookup
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    sprintf(str_port, "%i", config.servers[server_entry].port);
    result = getaddrinfo(config.servers[server_entry].hostname, str_port, &hints, &res);
    if (result != 0) {
        printf("Server: %s\n", config.servers[server_entry].hostname);
        perror("Not able to lookup server address");
        return 0;
    }
    int serverConn = socket(res[0].ai_family, res[0].ai_socktype, res[0].ai_protocol);

    // Need 1 second time out.. Inspired from http://developerweb.net/viewtopic.php?id=3196.
    // Set non-blocking
    if( (sock_arg = fcntl(serverConn, F_GETFL, NULL)) < 0) {
        fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
        return(0);
    }
    sock_arg |= O_NONBLOCK;
    if( fcntl(serverConn, F_SETFL, sock_arg) < 0) {
        fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
        return(0);
    }

    if (connect(serverConn, res[0].ai_addr, res[0].ai_addrlen) < 0) {
        if (errno == EINPROGRESS) {
            // fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
            do {
                tv.tv_sec = 1; // One second time out.
                tv.tv_usec = 0;
                FD_ZERO(&myset);
                FD_SET(serverConn, &myset);
                result = select(serverConn+1, NULL, &myset, NULL, &tv);
                if (result < 0 && errno != EINTR) {
                    fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
                    return(0);
                }
                else if (result > 0) {
                    // Socket selected for write
                    lon = sizeof(int);
                    if (getsockopt(serverConn, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                        fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
                        return(0);
                    }
                    // Check the value returned...
                    if (valopt) {
                        fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt)
                        );
                        return(0);
                    }
                    break;
                }
                else {
                    fprintf(stderr, "Timeout in select() - Cancelling!\n");
                    return(0);
                }
            } while (1);
        }
        else {
            fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
            return(0);
        }
    }

    // Set to blocking mode again...
    if( (sock_arg = fcntl(serverConn, F_GETFL, NULL)) < 0) {
        fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
        return 0;
    }
    sock_arg &= (~O_NONBLOCK);
    if( fcntl(serverConn, F_SETFL, sock_arg) < 0) {
        fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
        return 0;
    }

    return serverConn;
}

int sendChunk(int server_entry, FILE *fp, size_t chunk_size, int chunk_number, char *filename)
{
    char buf[MAXBUF];
    size_t nbytes;
    int serverConn;

    if ((serverConn = establishConnection(server_entry)) == 0) {
        printf("Failed to establish server connection\n");
        return -1;
    }


    bzero(buf, MAXBUF);

    // First line to server is "username password"
    sprintf(buf, "%s %s\r\n", config.username, config.password);
    write(serverConn, buf, strlen(buf));

    bzero(buf, MAXBUF);
    // second line is command and args, filename format for server is <dot>filename.<chunk number> 1..4
    sprintf(buf, "put .%s.%i\r\n", filename, chunk_number);
    write(serverConn, buf, strlen(buf));

    // Now lets write the data chunk, the file pointer has already been set to correct start pointer.
    while(chunk_size != 0) {
        if (chunk_size < MAXBUF) {
            nbytes = fread(buf, 1, chunk_size, fp);
        } else {
            nbytes = fread(buf, 1, MAXBUF, fp);
        }

        if (nbytes == 0 ) {
            break;
        }
        // TODO: Encrypt buffer here, before sending. But what to encrypt it with ?? User password could change..
        write(serverConn, buf, nbytes);
        chunk_size = chunk_size - nbytes;
    }
    close(serverConn);

    return 0;


}
/*
 * ids : the chunk ids for the file.
 * server: Server entry in the global config.
 * filename: The filename.
 * fp: FILE point to open file.
 */
int putChunk(struct chunk_ids ids, int server, char *filename, FILE *fp, size_t **chunk_sizes ) {

    size_t start_byte;

        // Two chunks for each server.
        for (int k = 0 ; k < 2; k ++) {
            // Calculate where to start in the file descriptor. By adding up the chunks size before it.
            // Our chunks fragments are denoted from 1..4, while our array is 0..3, so subtract one when
            // referencing into an array.

            start_byte = 0;
            for (int t = 0; t < ids.id[k] - 1; t++)
                start_byte += (size_t) chunk_sizes[t];

            fseek(fp, start_byte, SEEK_SET);
            sendChunk(server, fp, (size_t) chunk_sizes[ids.id[k] - 1], ids.id[k], filename);
        }
    return 0;
}

int putFile(char *filename) {
    FILE *fp;
    struct stat fs;
    size_t chunk_sizes[4];

    // Need to get rid of the '\n'
    filename[strlen(filename) -1 ] = '\0';
    if (filename == NULL) {
        printf("put command needs one argument.\n");
        return 0;
    }

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Not able to open file %s\n", filename);
        perror("-->");
        return 0;
    }

    // Get file length and divide by 4, to get chunk sizes of each of the 4 chunks.
    stat(filename, &fs);
    chunk_sizes[0] = chunk_sizes[1] = chunk_sizes[2] = fs.st_size / 4;
    chunk_sizes[3] = fs.st_size - ((fs.st_size / 4) * 3);

    // Get MD5
    int dist_entry = md5_checksum(fp);

    // Now send each chunk to the respective dfs servers.
    for (int server = 0; server < 4; server++) {
        putChunk(dist_table[dist_entry].ids[server], server, filename, fp, (size_t **) chunk_sizes);
    }

    fclose(fp);
    return 0;
}

int exitClient() {

    return 0;
}

void menu() {
    printf("<------------>\n");
    printf("'list'\n");
    printf("'get'\n");
    printf("'put'\n");
    printf("'exit'\n\n");
    printf("Enter command: ");
}

int main(int argc, char ** argv) {

    char buf[MAXBUF];

    if (argc != 2 ){
        printf("Usage: dfc [config file]\n");
        exit (-1);
    }
    init_config(argv[1]);

    while(1) {

        bzero(buf, MAXBUF);
        menu();
        fgets(buf, MAXBUF, stdin);

        if (!strncmp(buf, "exit", 4))
            return exitClient();
        else if (!strncmp(buf, "list", 4)) {
        listFiles();
        }
        else if(!strncmp(buf, "get", 3)) {
            strtok(buf, " ");
            getFile(strdup(strtok(NULL, " ")));
        }
        else if(!strncmp(buf, "put", 3)) {
            strtok(buf, " ");
            putFile(strdup(strtok(NULL, " ")));
        }
        else
            printf("\n COMMAND NOT RECOGNIZED \n");
    }
}
