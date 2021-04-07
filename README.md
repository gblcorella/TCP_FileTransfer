Gabriel Corella 
Computer Networks: Programming Assignment 1

Goal: Implement a client and a server which use TCP to transmit a file from the server to theclient. 
You may select a port number for the server that does not conflict with port numbers chosen by other students in the 
course. For convenience, both the hostname and the server port number may be hardcoded intoboth the client and server programs,
but this should be done in such a way that they are easy to change.

–  List of relevant files:
        - Makefile
        - Client.c
        - Server.c
        - Out.txt (Generated)
        - testFile / loremipsum (Testing)

–  Compilation instructions:
    In order to compile, open slit two terminal windows within project. Use makefile to compile both server and client 
    using 'make all'. This will generate the server and client .o files. In one bash terminal, type './server' to run the server first.
    Next, use the second bash terminal to run './client' and open the TCP connection. 

–  Configuration  file(s):
    Able to run on any Linux / Unix system.

–  Running instructions: 
    - TCP connections require that the server be ran first and then the client. 
    - Add in custom .txt files to local directory to send files.
