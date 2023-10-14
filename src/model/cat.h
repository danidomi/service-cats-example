#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <c-framework-service/error/error.h>

typedef struct Cat {
    int id;
    int age;
    char *name;
    Error *error;
} Cat;

Cat * new_error_cat(Error * err);
Cat * new_error_msg_cat(char * message);
char * to_string(Cat* cat);