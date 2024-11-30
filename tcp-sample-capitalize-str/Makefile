all:    server client

client: client.c
	gcc -Wall client.c -o client  

#	gcc -Wall client.c -o client  -lnsl

server: server.c
	gcc -Wall server.c -o server 

#	gcc -Wall server.c -o server  -lnsl

clean:
	rm client server

