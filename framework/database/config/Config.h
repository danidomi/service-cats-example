#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

// Define a structure to hold database configuration
typedef struct DatabaseConfig {
    char host[MAX_LINE_LENGTH];
    int port;
    char username[MAX_LINE_LENGTH];
    char password[MAX_LINE_LENGTH];
    char name[MAX_LINE_LENGTH];
}DatabaseConfig;

DatabaseConfig * readPropertyFile(const char *filename);