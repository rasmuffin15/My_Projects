CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lpthread

OBJS = web_server.o 

all: webserver

web_server.o: web_server.c
	$(CC) $(CFLAGS) -c web_server.c

webserver: web_server.o
	$(CC) $(LDFLAGS) -o $@ web_server.o

clean:
	rm -f web_server.o webserver
