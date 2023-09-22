all: Server

Server: Server.o Cat.o Service.o CatController.o
	gcc -o Server Server.o Cat.o Service.o CatController.o

Server.o: Server.c
	gcc -c Server.c

Cat.o: src/model/Cat.c
	gcc -c src/model/Cat.c

CatController.o: src/controller/CatController.c
	gcc -c src/controller/CatController.c

Service.o: src/service/Service.c
	gcc -c src/model/Repository.h src/service/Service.c

clean:
	rm -rf *.o server
