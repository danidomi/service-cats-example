#include "cat.h"

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

char * to_string(Cat * cat) {
    char result[100];
    sprintf(result, "{\"name\":\"%s\",\"age\":%d}",cat->name, cat->age);
    return result;
}