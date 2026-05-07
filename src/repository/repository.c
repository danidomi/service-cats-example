#include "repository.h"

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
