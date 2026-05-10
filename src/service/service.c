#include "service.h"

#include <string.h>

#include "../metrics/app_metrics.h"

Cat *create_cat(int age, char *name, Error **err) {
    if (age <= 0 || !name || strlen(name) == 0) {
        if (err) *err = error_new("invalid input: age must be > 0 and name non-empty");
        return NULL;
    }
    Cat *c = persist_cat(age, name, err);
    if (c) app_metrics_cat_created();
    return c;
}

Cat *get_cat(int id, Error **err) {
    if (id <= 0) {
        if (err) *err = error_new("invalid id");
        return NULL;
    }
    return find_cat(id, err);
}

Cat **list_all_cats(size_t *count, Error **err) {
    return list_cats(count, err);
}

Cat *modify_cat(int id, int age, char *name, Error **err) {
    if (id <= 0) {
        if (err) *err = error_new("invalid id");
        return NULL;
    }
    if (age <= 0 || !name || strlen(name) == 0) {
        if (err) *err = error_new("invalid input: age must be > 0 and name non-empty");
        return NULL;
    }
    return update_cat(id, age, name, err);
}

int remove_cat(int id, Error **err) {
    if (id <= 0) {
        if (err) *err = error_new("invalid id");
        return -1;
    }
    int rc = delete_cat(id, err);
    if (rc == 1) app_metrics_cat_deleted();
    return rc;
}
