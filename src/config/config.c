#include "config.h"

#include <stdlib.h>
#include <string.h>

static char *getenv_or(const char *key, const char *fallback) {
    const char *v = getenv(key);
    return strdup(v && *v ? v : fallback);
}

DbConfig db_config_from_env(void) {
    DbConfig cfg = {0};
    cfg.host     = getenv_or("MYSQL_HOST",     "127.0.0.1");
    cfg.user     = getenv_or("MYSQL_USERNAME", "root");
    cfg.password = getenv_or("MYSQL_PASSWORD", "root");
    cfg.database = getenv_or("MYSQL_DATABASE", "cats");

    const char *port = getenv("MYSQL_PORT");
    if (!port || !*port) port = getenv("MYSQL_TCP_PORT");
    cfg.port = (port && *port) ? atoi(port) : 3306;

    return cfg;
}

void db_config_free(DbConfig *cfg) {
    if (!cfg) return;
    free(cfg->host);
    free(cfg->user);
    free(cfg->password);
    free(cfg->database);
    cfg->host = cfg->user = cfg->password = cfg->database = NULL;
}

int port_from_env(int default_port) {
    const char *p = getenv("PORT");
    return (p && *p) ? atoi(p) : default_port;
}
