#pragma once

typedef struct Error {
    char * message;
} Error;

Error * new(char * message);