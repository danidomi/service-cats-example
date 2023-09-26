#include "Repository.h"

// Global database connection handle
MYSQL *conn;

Error * configDatabase() {
    if (conn != NULL) {
        return NULL;
    }

    struct DatabaseConfig *config = readPropertyFile("config/database.properties");
    if (config == NULL) {
        return new("Error while reading the config file");
    }
    conn = mysql_init(NULL);

    if (conn == NULL) {
        char errorMessage[100];
        sprintf(errorMessage, "Error while connecting to mysql %u: %s\n", mysql_errno(conn), mysql_error(conn));
        logMessage(ERROR, errorMessage);
        return new(errorMessage);
    }
    logMessage(DEBUG, "After");
    if (mysql_real_connect(conn, config->host, config->username,
                           config->password, config->name, config->port, NULL, 0) == NULL) {
        logMessage(ERROR,"Error in connection %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error while connection");
    }

    logMessage(DEBUG, "after ");

    if (mysql_query(conn, "use cats")) {
        logMessage(ERROR,"Error checking database %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error checking database");
    }
}

Cat *FindCat(int id) {
    logMessage(DEBUG, "about to fetch a cat");
    configDatabase();
    // Check if the connection is valid
    if (conn == NULL) {
        logMessage(ERROR, "Database connection is not initialized");
        return NULL;
    }
    MYSQL_RES *result = NULL;
    Cat *cat = NULL;

    logMessage(ERROR, "before mysql_query");

    // Construct the SQL query to retrieve the cat with the specified id
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "SELECT id, name, age FROM cats WHERE id = %d", id);

    // Execute the SQL query and store the result
    if (mysql_query(conn, query) != 0) {
        logMessage(ERROR, "Query error: %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    logMessage(DEBUG, "after mysql_query");
    result = mysql_store_result(conn);

    if (result == NULL) {
        logMessage(ERROR, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Check if a row was retrieved
    MYSQL_ROW row = mysql_fetch_row(result);
    logMessage(DEBUG, "after mysql_fetch_row");

    if (row != NULL) {
        // Allocate memory for the Cat object
        cat = (Cat *) malloc(sizeof(Cat));

        if (cat == NULL) {
            perror("Memory allocation failed");
            mysql_free_result(result);
            return NULL;
        }

        // Parse the values from the result row
        cat->id = atoi(row[0]);

        // Check if row[1] (name) is NULL before using strdup
        if (row[1] != NULL) {
            cat->name = strdup(row[1]);
            if (cat->name == NULL) {
                perror("Memory allocation failed");
                free(cat);
                mysql_free_result(result);
                return NULL;
            }
        } else {
            cat->name = NULL; // Handle case where name is NULL in the database
        }

        cat->age = atoi(row[2]);
    }
    logMessage(DEBUG, "cat %d %d %s", cat->id, cat->age, cat->name);

    // Free the result set
    mysql_free_result(result);

    return cat;
}


Cat *PersistCat(int age, char *name) {
    Error * err = configDatabase();
    if (err != NULL){
        logMessage(ERROR, "unable to get config database");
        return newErrorCat(err);
    }
    // Check if the connection is valid
    if (conn == NULL) {
        logMessage(ERROR, "Database connection is not initialized");
        return newErrorMsgCat("Database connection is not initialized");
    }

    // Construct the SQL query to insert a new cat record
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "INSERT INTO cats (name, age) VALUES ('%s', %d)", name, age);

    // Execute the SQL query
    if (mysql_query(conn, query)) {
        logMessage(ERROR, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Retrieve the ID of the newly inserted cat
    int newCatId = mysql_insert_id(conn);

    // Create a Cat object with the inserted data
    Cat *newCat = (Cat *) malloc(sizeof(Cat));
    if (newCat == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    newCat->id = newCatId;
    newCat->name = strdup(name);
    newCat->age = age;

    return newCat;
}
