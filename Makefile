.PHONY: all clean service-cats-example converter.o

all: service-cats-example

CFLAGS += -Ideps

output_folder := output  # Define the output folder

# Create the output folder if it doesn't exist
$(shell mkdir -p $(output_folder))

service-cats-example: cat.o repository.o converter.o service.o controller.o
	gcc -o bin/service-cats-example output/controller.o output/converter.o output/cat.o output/repository.o output/service.o deps/c-framework-service/c-framework-service.o -lmysqlclient $(CFLAGS)

converter.o: src/utils/converter.c
	gcc -c src/utils/converter.c -o output/converter.o $(CFLAGS)

controller.o: src/controller/controller.c
	gcc -c src/controller/controller.c -o output/controller.o $(CFLAGS)

service.o: src/service/service.c
	gcc -c src/service/service.c -o output/service.o $(CFLAGS)

cat.o: src/model/cat.c
	gcc -c src/model/cat.c -o output/cat.o $(CFLAGS)

repository.o: src/repository/repository.c
	gcc -c src/repository/repository.c -o output/repository.o $(CFLAGS)

clean-deps:
	rm -rf deps/*

clean:
	rm -rf output/*.o bin/service-cats-example
