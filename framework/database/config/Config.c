#include "Config.h"

// Function to read and parse the property file
DatabaseConfig * read_property_file(const char *filename) {
    DatabaseConfig * config = malloc(sizeof(DatabaseConfig));
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening property file");
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        // Split the line into key and value
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key != NULL && value != NULL) {
            // Trim leading and trailing whitespace from the value
            value = strtok(value, "\r");
            value = strtok(value, "\n");
            while (value[0] == ' ') {
                value++;
            }
            while (value[strlen(value) - 1] == ' ') {
                value[strlen(value) - 1] = '\0';
            }

            // Assign values based on the key
            if (strcmp(key, "db_host") == 0) {
                strcpy(config->host, value);
            } else if (strcmp(key, "db_port") == 0) {
                config->port = atoi(value);
            } else if (strcmp(key, "db_username") == 0) {
                strcpy(config->username, value);
            } else if (strcmp(key, "db_password") == 0) {
                strcpy(config->password, value);
            } else if (strcmp(key, "db_name") == 0) {
                strcpy(config->name, value);
            }
        }
    }

    fclose(file);
    return config;
}