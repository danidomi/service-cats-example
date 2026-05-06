#include "service.h"

#include <string.h>

Cat *create_cat(int age, char *name, Error **err) {
    if (age <= 0 || !name || strlen(name) == 0) {
        if (err) *err = error_new("invalid input: age must be > 0 and name non-empty");
        return NULL;
    }
    return persist_cat(age, name, err);
}

Cat *get_cat(int id, Error **err) {
    if (id <= 0) {
        if (err) *err = error_new("invalid id");
        return NULL;
    }
    return find_cat(id, err);
}
