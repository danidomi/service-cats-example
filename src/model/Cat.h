#pragma once

#include "../../framework/error/Error.h"

typedef struct Cat {
    int id;
    int age;
    char *name;
    Error *error;
} Cat;

Cat * newErrorCat(Error * err);
Cat * newErrorMsgCat(char * message);