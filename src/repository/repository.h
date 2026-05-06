#pragma once

#include <mysql/mysql.h>
#include <stdio.h>

#include <c-framework-service/database/config/config.h>
#include <c-framework-service/error/error.h>
#include <c-framework-service/logger/logger.h>

#include "../model/cat.h"

/* Return NULL when the cat is simply not found (no row).
 * If err is non-NULL and a real error occurred (DB/network), *err is set. */
Cat *find_cat(int id, Error **err);
Cat *persist_cat(int age, char *name, Error **err);
