#include "cat.h"

#include <string.h>

Cat *new_error_cat(Error *err) {
    Cat *cat = calloc(1, sizeof(Cat));
    if (!cat) return NULL;
    cat->error = err;
    return cat;
}

Cat *new_error_msg_cat(char *message) {
    Cat *cat = calloc(1, sizeof(Cat));
    if (!cat) return NULL;
    cat->error = error_new(message);
    return cat;
}

/* Caller owns the returned string and must free() it. */
char *to_string(Cat *cat) {
    if (!cat) return NULL;
    size_t need = (cat->name ? strlen(cat->name) : 0) + 64;
    char *buf = malloc(need);
    if (!buf) return NULL;
    snprintf(buf, need, "{\"name\":\"%s\",\"age\":%d}",
             cat->name ? cat->name : "", cat->age);
    return buf;
}
