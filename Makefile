all: Server

Server: Server.o Request.o Response.o Logger.o Error.o Converter.o Config.o Cat.o Repository.o Service.o Controller.o
	gcc -o bin/Server output/*.o -lmysqlclient

Request.o: framework/request/Request.c
	gcc -c framework/request/Request.c -o output/Request.o

Response.o: framework/response/Response.c
	gcc -c framework/response/Response.c -o output/Response.o

Server.o: framework/Server.c
	gcc -c framework/Server.c -o output/Server.o

Logger.o: framework/logger/Logger.c
	gcc -c framework/logger/Logger.c -o output/Logger.o

Converter.o: src/utils/Converter.c
	gcc -c src/utils/Converter.c -o output/Converter.o

Config.o: framework/database/config/Config.c
	gcc -c framework/database/config/Config.c -o output/Config.o

Error.o: framework/error/Error.c
	gcc -c framework/error/Error.c -o output/Error.o

Controller.o: src/controller/Controller.c
	gcc -c src/controller/Controller.c -o output/Controller.o

Service.o: src/service/Service.c
	gcc -c src/service/Service.c -o output/Service.o

Cat.o: src/model/Cat.c
	gcc -c src/model/Cat.c -o output/Cat.o

Repository.o: src/repository/Repository.c
	gcc -c src/repository/Repository.c -o output/Repository.o

clean:
	rm -rf output/*.o bin/Server
