CC= /usr/bin/gcc

all:	client server

client: client.c;
	gcc client.c -o client

server: server.c;
	gcc server.c -o server

clean:
	rm -rf client server out.txt
