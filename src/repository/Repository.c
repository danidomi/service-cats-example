#include "Repository.h"

// Global database connection handle
MYSQL *conn;

Error * configDatabase() {
    if (conn != NULL) {
        return NULL;
    }

    struct DatabaseConfig *config = read_property_file("config/database.properties");
    if (config == NULL) {
        return new("Error while reading the config file");
    }
    conn = mysql_init(NULL);

    if (conn == NULL) {
        char errorMessage[100];
        sprintf(errorMessage, "Error while connecting to mysql %u: %s\n", mysql_errno(conn), mysql_error(conn));
        log_message(ERROR, errorMessage);
        return new(errorMessage);
    }
    log_message(DEBUG, "After");
    if (mysql_real_connect(conn, "127.0.0.1", config->username,
                           config->password, config->name, 3307, NULL, 0) == NULL) {
        log_message(ERROR,"Error in connection %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error while connection");
    }

    log_message(DEBUG, "after ");

    if (mysql_query(conn, "use cats")) {
        log_message(ERROR,"Error checking database %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error checking database");
    }
}

Cat *find_cat(int id) {
    log_message(DEBUG, "about to fetch a cat");
    configDatabase();
    // Check if the connection is valid
    if (conn == NULL) {
        log_message(ERROR, "Database connection is not initialized");
        return NULL;
    }
    MYSQL_RES *result = NULL;
    Cat *cat = NULL;

    log_message(ERROR, "before mysql_query");

    // Construct the SQL query to retrieve the cat with the specified id
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "SELECT id, name, age FROM cats WHERE id = %d", id);

    // Execute the SQL query and store the result
    if (mysql_query(conn, query) != 0) {
        log_message(ERROR, "Query error: %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    log_message(DEBUG, "after mysql_query");
    result = mysql_store_result(conn);

    if (result == NULL) {
        log_message(ERROR, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Check if a row was retrieved
    MYSQL_ROW row = mysql_fetch_row(result);
    log_message(DEBUG, "after mysql_fetch_row");

    if (row != NULL) {
        // Allocate memory for the Cat object
        cat = (Cat *) malloc(sizeof(Cat));

        if (cat == NULL) {
            log_message(ERROR, "Memory allocation failed");
            mysql_free_result(result);
            return NULL;
        }

        // Parse the values from the result row
        cat->id = atoi(row[0]);

        // Check if row[1] (name) is NULL before using strdup
        if (row[1] != NULL) {
            cat->name = strdup(row[1]);
            if (cat->name == NULL) {
                log_message(ERROR, "Memory allocation failed");
                free(cat);
                mysql_free_result(result);
                return NULL;
            }
        } else {
            cat->name = NULL; // Handle case where name is NULL in the database
        }

        cat->age = atoi(row[2]);
    }
    log_message(DEBUG, "cat %d %d %s", cat->id, cat->age, cat->name);

    // Free the result set
    mysql_free_result(result);

    return cat;
}


Cat *persist_cat(int age, char *name) {
    Error * err = configDatabase();
    if (err != NULL){
        log_message(ERROR, "unable to get config database");
        return new_error_cat(err);
    }
    // Check if the connection is valid
    if (conn == NULL) {
        log_message(ERROR, "Database connection is not initialized");
        return new_error_msg_cat("Database connection is not initialized");
    }

    // Construct the SQL query to insert a new cat record
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "INSERT INTO cats (name, age) VALUES ('%s', %d)", name, age);

    // Execute the SQL query
    if (mysql_query(conn, query)) {
        log_message(ERROR, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Retrieve the ID of the newly inserted cat
    int newCatId = mysql_insert_id(conn);

    // Create a Cat object with the inserted data
    Cat *newCat = (Cat *) malloc(sizeof(Cat));
    if (newCat == NULL) {
        log_message(ERROR, "Memory allocation failed");
        return NULL;
    }

    newCat->id = newCatId;
    newCat->name = strdup(name);
    newCat->age = age;

    return newCat;
}
