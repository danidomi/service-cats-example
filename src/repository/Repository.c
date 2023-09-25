#include "Repository.h"

// Global database connection handle
MYSQL *conn;

Error * configDatabase() {
    if (conn != NULL) {
        return NULL;
    }

    struct DatabaseConfig *config = readPropertyFile("/Users/danidomi/Projects/MVC/config/database.properties");
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

    if (mysql_real_connect(conn, "127.0.0.1", "root",
                           "root", "", 3307, NULL, 0) == NULL) {
        printf("Error in connection %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error while connection");
    }

    if (mysql_query(conn, "use cats")) {
        printf("Error checking database %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return new("Error checking database");
    }
}

Cat *FindCat(int id) {
    logMessage(DEBUG, "about to fetch a cat");
    configDatabase();
    // Check if the connection is valid
    if (conn == NULL) {
        fprintf(stderr, "Database connection is not initialized\n");
        return NULL;
    }
    // Initialize a Cat pointer
    Cat *cat = NULL;

    // Construct the SQL query to retrieve the cat with the specified id
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "SELECT id, name, age FROM cats WHERE id = %d", id);

    // Execute the SQL query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Get the result set
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return NULL;
    }

    // Check if a row was retrieved
    MYSQL_ROW row = mysql_fetch_row(result);
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
        cat->name = strdup(row[1]);
        cat->age = atoi(row[2]);
    }

    // Free the result set
    mysql_free_result(result);

    return cat;
}


Cat *PersistCat(int age, char *name) {
    Error * err = configDatabase();
    if (err != NULL){
        return newErrorCat(err);
    }
    // Check if the connection is valid
    if (conn == NULL) {
        return newErrorMsgCat("Database connection is not initialized");
    }

    // Construct the SQL query to insert a new cat record
    char query[100]; // Adjust the size according to your query
    snprintf(query, sizeof(query), "INSERT INTO cats (name, age) VALUES ('%s', %d)", name, age);

    // Execute the SQL query
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
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
