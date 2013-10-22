all: Server

Server: Server.o Cat.o 
	gcc Server.o Cat.o -o server

Server.o: server.c
	gcc -c Server.c

Cat.o: Cat.c
	gcc -c Cat.c Cat.h ICat.h

clean:
	rm -rf *.o server
