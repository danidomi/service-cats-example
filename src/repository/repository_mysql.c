#include "repository.h"

#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>

static MYSQL *conn = NULL;
static DbConfig cfg = {0};

void repository_init(const DbConfig *src) {
    if (!src) return;
    db_config_free(&cfg);
    cfg.host     = src->host     ? strdup(src->host)     : NULL;
    cfg.user     = src->user     ? strdup(src->user)     : NULL;
    cfg.password = src->password ? strdup(src->password) : NULL;
    cfg.database = src->database ? strdup(src->database) : NULL;
    cfg.port     = src->port;
}

void repository_close(void) {
    if (conn) {
        mysql_close(conn);
        conn = NULL;
    }
    db_config_free(&cfg);
}

static void set_err(Error **err, const char *msg) {
    if (err && !*err) {
        *err = error_new(msg);
    }
}

static Error *configDatabase(void) {
    if (conn != NULL) return NULL;

    if (!cfg.host) {
        return error_new("repository_init has not been called");
    }

    conn = mysql_init(NULL);
    if (conn == NULL) {
        return error_new("mysql_init failed");
    }

    if (mysql_real_connect(conn,
                           cfg.host, cfg.user, cfg.password, cfg.database,
                           cfg.port, NULL, 0) == NULL) {
        char buf[256];
        snprintf(buf, sizeof(buf), "mysql_real_connect: %s", mysql_error(conn));
        log_message(ERROR, "%s", buf);
        Error *e = error_new(buf);
        mysql_close(conn);
        conn = NULL;
        return e;
    }

    log_message(INFO, "connected to mysql at %s:%d/%s as %s",
                cfg.host, cfg.port, cfg.database, cfg.user);
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

Cat **list_cats(size_t *count, Error **err) {
    if (count) *count = 0;

    Error *cfg_err = configDatabase();
    if (cfg_err) {
        if (err) *err = cfg_err; else error_free(cfg_err);
        return NULL;
    }

    if (mysql_query(conn, "SELECT id, name, age FROM cats ORDER BY id") != 0) {
        log_message(ERROR, "list query: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        log_message(ERROR, "list store_result: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }

    my_ulonglong n = mysql_num_rows(result);
    if (n == 0) {
        mysql_free_result(result);
        return NULL;
    }

    Cat **arr = malloc((size_t)n * sizeof(Cat *));
    if (!arr) {
        mysql_free_result(result);
        set_err(err, "out of memory");
        return NULL;
    }

    size_t i = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != NULL && i < (size_t)n) {
        Cat *c = calloc(1, sizeof(Cat));
        if (!c) {
            cats_free(arr, i);
            mysql_free_result(result);
            set_err(err, "out of memory");
            return NULL;
        }
        c->id = row[0] ? atoi(row[0]) : 0;
        c->name = row[1] ? strdup(row[1]) : NULL;
        c->age = row[2] ? atoi(row[2]) : 0;
        arr[i++] = c;
    }
    mysql_free_result(result);
    if (count) *count = i;
    return arr;
}

void cats_free(Cat **cats, size_t count) {
    if (!cats) return;
    for (size_t i = 0; i < count; i++) {
        if (!cats[i]) continue;
        free(cats[i]->name);
        free(cats[i]);
    }
    free(cats);
}

Cat *update_cat(int id, int age, char *name, Error **err) {
    Error *cfg_err = configDatabase();
    if (cfg_err) {
        if (err) *err = cfg_err; else error_free(cfg_err);
        return NULL;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "UPDATE cats SET name='%s', age=%d WHERE id=%d",
             name ? name : "", age, id);

    if (mysql_query(conn, query) != 0) {
        log_message(ERROR, "update: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return NULL;
    }
    if (mysql_affected_rows(conn) == 0) {
        return NULL; /* not found, no error */
    }

    Cat *cat = malloc(sizeof(Cat));
    if (!cat) {
        set_err(err, "out of memory");
        return NULL;
    }
    cat->id = id;
    cat->name = name ? strdup(name) : NULL;
    cat->age = age;
    cat->error = NULL;
    return cat;
}

int delete_cat(int id, Error **err) {
    Error *cfg_err = configDatabase();
    if (cfg_err) {
        if (err) *err = cfg_err; else error_free(cfg_err);
        return -1;
    }

    char query[128];
    snprintf(query, sizeof(query), "DELETE FROM cats WHERE id=%d", id);

    if (mysql_query(conn, query) != 0) {
        log_message(ERROR, "delete: %s", mysql_error(conn));
        set_err(err, mysql_error(conn));
        return -1;
    }
    return mysql_affected_rows(conn) > 0 ? 1 : 0;
}
