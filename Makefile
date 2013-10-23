all: Server

Server: Server.o Cat.o 
	gcc -o Server Server.o Cat.o 

Server.o: server.c
	gcc -c Server.c

Cat.o: Cat.c
	gcc -c Cat.c

clean:
	rm -rf *.o server
