.PHONY: all clean Server Converter.o

all: Server

CFLAGS += -Ideps

Server: Cat.o Repository.o Converter.o Service.o Controller.o
	gcc -o bin/Server output/Controller.o output/Converter.o output/Cat.o output/Repository.o output/Service.o deps/c-framework-service/c-framework-service.o -lmysqlclient $(CFLAGS)

Converter.o: src/utils/Converter.c
	gcc -c src/utils/Converter.c -o output/Converter.o $(CFLAGS)

Controller.o: src/controller/Controller.c
	gcc -c -u get_path -u handle_api src/controller/Controller.c -o output/Controller.o $(CFLAGS)

Service.o: src/service/Service.c
	gcc -c src/service/Service.c -o output/Service.o $(CFLAGS)

Cat.o: src/model/Cat.c
	gcc -c src/model/Cat.c -o output/Cat.o $(CFLAGS)

Repository.o: src/repository/Repository.c
	gcc -c src/repository/Repository.c -o output/Repository.o $(CFLAGS)

clean-deps:
	rm -rf deps/*

clean:
	rm -rf output/*.o bin/Server
