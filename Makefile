.PHONY: all clean clean-deps service-cats-example

CC ?= gcc
CFLAGS += -Ideps

OUTPUT_DIR := output
BIN_DIR := bin
FRAMEWORK_OBJ := deps/c-framework-service/c-framework-service.o
LDLIBS := -lmysqlclient -lpthread

OBJS := \
    $(OUTPUT_DIR)/cat.o \
    $(OUTPUT_DIR)/repository.o \
    $(OUTPUT_DIR)/converter.o \
    $(OUTPUT_DIR)/service.o \
    $(OUTPUT_DIR)/controller.o \
    $(OUTPUT_DIR)/main.o

all: service-cats-example

$(OUTPUT_DIR) $(BIN_DIR):
	mkdir -p $@

service-cats-example: $(OUTPUT_DIR) $(BIN_DIR) $(OBJS) $(FRAMEWORK_OBJ)
	$(CC) -o $(BIN_DIR)/service-cats-example $(OBJS) $(FRAMEWORK_OBJ) $(LDLIBS) $(CFLAGS)

$(OUTPUT_DIR)/main.o: main.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/converter.o: src/utils/converter.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/controller.o: src/controller/controller.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/service.o: src/service/service.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/cat.o: src/model/cat.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/repository.o: src/repository/repository.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean-deps:
	rm -rf deps/*

clean:
	rm -rf $(OUTPUT_DIR) $(BIN_DIR)
