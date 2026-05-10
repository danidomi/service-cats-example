.PHONY: all clean clean-deps service-cats-example

CC ?= gcc
CFLAGS += -Ideps

OUTPUT_DIR := output
BIN_DIR := bin
FRAMEWORK_OBJ := deps/c-framework-service/c-framework-service.o

# Repository backend: memory (default, no external DB), mysql, or postgres.
# To add postgres: drop src/repository/repository_postgres.c implementing
# repository.h and add an `else ifeq ($(DB),postgres)` branch below.
DB ?= memory

ifeq ($(DB),mysql)
    REPO_SRC := src/repository/repository_mysql.c
    LDLIBS   := -lmysqlclient -lpthread
else ifeq ($(DB),memory)
    REPO_SRC := src/repository/repository_memory.c
    LDLIBS   := -lpthread
else
    $(error Unknown DB backend "$(DB)" — use memory or mysql)
endif

OBJS := \
    $(OUTPUT_DIR)/config.o \
    $(OUTPUT_DIR)/cat.o \
    $(OUTPUT_DIR)/repository.o \
    $(OUTPUT_DIR)/converter.o \
    $(OUTPUT_DIR)/json_input.o \
    $(OUTPUT_DIR)/app_metrics.o \
    $(OUTPUT_DIR)/service.o \
    $(OUTPUT_DIR)/controller.o \
    $(OUTPUT_DIR)/openapi.o \
    $(OUTPUT_DIR)/main.o

all: service-cats-example

$(OUTPUT_DIR) $(BIN_DIR):
	mkdir -p $@

service-cats-example: $(OUTPUT_DIR) $(BIN_DIR) $(OBJS) $(FRAMEWORK_OBJ)
	$(CC) -o $(BIN_DIR)/service-cats-example $(OBJS) $(FRAMEWORK_OBJ) $(LDLIBS) $(CFLAGS)

$(OUTPUT_DIR)/main.o: main.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/config.o: src/config/config.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/converter.o: src/utils/converter.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/json_input.o: src/utils/json_input.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/controller.o: src/controller/controller.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/service.o: src/service/service.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/cat.o: src/model/cat.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/repository.o: $(REPO_SRC)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/openapi.o: src/openapi/openapi.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(OUTPUT_DIR)/app_metrics.o: src/metrics/app_metrics.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean-deps:
	rm -rf deps/*

clean:
	rm -rf $(OUTPUT_DIR) $(BIN_DIR)
