#include <stdlib.h>
#include "Cat.h"

Cat * newErrorCat(Error * err) {
    Cat * cat = malloc(sizeof (Cat));
    cat->error = err;
    return cat;
}

Cat * newErrorMsgCat(char * message) {
    Cat * cat = malloc(sizeof (Cat));
    cat->error = new(message);
    return cat;
}