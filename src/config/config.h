#pragma once

typedef struct {
    char *host;
    int port;
    char *user;
    char *password;
    char *database;
} DbConfig;

DbConfig db_config_from_env(void);
void db_config_free(DbConfig *cfg);

int port_from_env(int default_port);
