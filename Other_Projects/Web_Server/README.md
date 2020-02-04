Thomas Rasmussen
CSCI 4273
Programing Assignment Two


!!INSTRUCTIONS ON HOW TO RUN!!

1) Open up a terminal and navigate to the directory that this file is stored
2) Type 'make' to compile 'web_server.c'
3) Type './webserver 8088' to run the web server
4) Open up a web browser and type 'localhost:8088' and you will see the root web page on my server

My web server is written in 'C'. The server waits to receive a client connection and when it does it 
creates a new thread which goes ahead and establishes a connection between the client and the server. 
This thread also finds out what the request type was of the HTTP request and calls the appriopiate function.
In this case I only deal with GET request. This thread then retreaves the requested files or sends back a 
'404' error in the event the file could not be found.