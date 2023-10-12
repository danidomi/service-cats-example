#include <stdlib.h>
#include "Cat.h"

Cat * new_error_cat(Error * err) {
    Cat * cat = malloc(sizeof (Cat));
    cat->error = err;
    return cat;
}

Cat * new_error_msg_cat(char * message) {
    Cat * cat = malloc(sizeof (Cat));
    cat->error = new(message);
    return cat;
}