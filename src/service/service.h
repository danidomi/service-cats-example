#pragma once

#include "../repository/repository.h"

Cat *create_cat(int age, char *name, Error **err);
Cat *get_cat(int id, Error **err);
