#include "repository.h"

#include <string.h>

static MYSQL *conn = NULL;

static void set_err(Error **err, const char *msg) {
    if (err && !*err) {
        *err = error_new(msg);
    }
}

static Error *configDatabase(void) {
    if (conn != NULL) return NULL;

    conn = mysql_init(NULL);
    if (conn == NULL) {
        return error_new("mysql_init failed");
    }

    if (mysql_real_connect(conn, "127.0.0.1", "root", "root", "cats", 3307, NULL, 0) == NULL) {
        char buf[256];
        snprintf(buf, sizeof(buf), "mysql_real_connect: %s", mysql_error(conn));
        log_message(ERROR, "%s", buf);
        Error *e = error_new(buf);
        mysql_close(conn);
        conn = NULL;
        return e;
    }

    if (mysql_query(conn, "use cats")) {
        char buf[256];
        snprintf(buf, sizeof(buf), "use cats: %s", mysql_error(conn));
        log_message(ERROR, "%s", buf);
        return error_new(buf);
    }
    return NULL;
}

Cat *find_cat(int id, Error **err) {
    Error *cfg_err = configDatabase();
    if (cfg_err) {
        if (err) *err = cfg_err; else error_free(cfg_err);
        return NULL;
    }

    char query[128];
    snprintf(query, sizeof(query), "SELECT id, name, age FROM cats WHERE id = %d", id);

    if (mysql_query(conn, query) != 0) {
        log_message(ERROR, "query: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        log_message(ERROR, "store_result: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }

    Cat *cat = NULL;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row != NULL) {
        cat = malloc(sizeof(Cat));
        if (!cat) {
            set_err(err, "out of memory");
            goto cleanup;
        }
        cat->id = atoi(row[0]);
        cat->name = row[1] ? strdup(row[1]) : NULL;
        cat->age = row[2] ? atoi(row[2]) : 0;
        cat->error = NULL;
    }
    /* row == NULL is "not found", not an error: cat stays NULL, err stays unset */

cleanup:
    mysql_free_result(result);
    return cat;
}

Cat *persist_cat(int age, char *name, Error **err) {
    Error *cfg_err = configDatabase();
    if (cfg_err) {
        if (err) *err = cfg_err; else error_free(cfg_err);
        return NULL;
    }

    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO cats (name, age) VALUES ('%s', %d)", name, age);

    if (mysql_query(conn, query)) {
        log_message(ERROR, "insert: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }

    Cat *cat = malloc(sizeof(Cat));
    if (!cat) {
        set_err(err, "out of memory");
        return NULL;
    }
    cat->id = (int)mysql_insert_id(conn);
    cat->name = strdup(name);
    cat->age = age;
    cat->error = NULL;
    return cat;
}
