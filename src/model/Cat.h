#pragma once

#include "../../framework/error/Error.h"

typedef struct Cat {
    int id;
    int age;
    char *name;
    Error *error;
} Cat;

Cat * new_error_cat(Error * err);
Cat * new_error_msg_cat(char * message);