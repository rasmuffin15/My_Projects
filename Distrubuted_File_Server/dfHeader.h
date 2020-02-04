//
// Created by Thomas Rasmussen on 12/1/19.
//

#ifndef FOUR_DFHEADER_H
#define FOUR_DFHEADER_H

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

#define MAXLINE  2048  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

#define TRUE 1;
#define FALSE 0;

struct dfsserver {
    char hostname[64];
    int port;
};

typedef struct dfsserver dfs_server;

struct dfsconfig {
    dfs_server servers[4];
    char username[64];
    char password[64];
};

struct chunk_ids {
    int id[2];
};

struct distribution {
    struct chunk_ids ids[4];
};

typedef struct fn filenode;

struct fn {
    char *name;         // Name of file.
    char fragments[4];  // Fragments present.
    filenode *next; // Linked list to next entry.
};



/* Globals */
int listenfd;
int clientlen; /* byte size of client's address */
struct sockaddr_in clientaddr; /* client addr */
char working_dir[80]; // Name of dir for dfs server.


void error(char *);
void menu();
void listFiles();
void getFiles();
int putFile( char *);
void requestHandler(int);
int establishConnection(int);

int md5_checksum(FILE *);
int putChunk(struct chunk_ids , int, char *, FILE *, size_t *[]);
int sendChunk(int , FILE *, size_t, int, char *);

int open_listenfd(int);
int exitClient();
int validateUser(char*, char*);


int fn_getfragments( filenode *fn);
filenode *fn_findbyname(filenode **root, char *name);
void fn_setframent(filenode *fn, int fragment);
filenode *fn_new_filenode(char *name);
void fn_push(filenode *fn, filenode **root);
void fn_printall(filenode **root);
void fn_cleanup(filenode **root);

#endif //FOUR_DFHEADER_H
