#pragma once

#include "../repository/repository.h"

Cat *create_cat(int age, char *name, Error **err);
Cat *get_cat(int id, Error **err);
Cat **list_all_cats(size_t *count, Error **err);
Cat *modify_cat(int id, int age, char *name, Error **err);
int remove_cat(int id, Error **err);
