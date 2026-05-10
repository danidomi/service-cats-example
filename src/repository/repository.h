#pragma once

#include <stdio.h>

#include <c-framework-service/error/error.h>
#include <c-framework-service/logger/logger.h>

#include "../config/config.h"
#include "../model/cat.h"

void repository_init(const DbConfig *cfg);
void repository_close(void);

/* Return NULL when the cat is simply not found (no row).
 * If err is non-NULL and a real error occurred (DB/network), *err is set. */
Cat *find_cat(int id, Error **err);
Cat *persist_cat(int age, char *name, Error **err);

/* Returns a malloc'd array of Cat* (each Cat malloc'd) and writes the count.
 * On empty result returns NULL with *count == 0 (not an error).
 * Caller frees with cats_free(). */
Cat **list_cats(size_t *count, Error **err);
void cats_free(Cat **cats, size_t count);

/* update: returns the updated Cat (caller frees); NULL when id not found.
 * delete: 1 if deleted, 0 if not found, -1 on error (with *err set). */
Cat *update_cat(int id, int age, char *name, Error **err);
int delete_cat(int id, Error **err);
