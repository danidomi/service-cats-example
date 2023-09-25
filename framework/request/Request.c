#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Request.h"

Request *parseRequest(char *req) {
    if (req == NULL) {
        return NULL;
    }

    // Allocate memory for the Request struct
    Request *request = (Request *)malloc(sizeof(Request));
    if (request == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Initialize query parameters
    request->queryParams = NULL;
    request->queryParamsCount = 0;

    // Parse the method (e.g., "GET")
    char *methodEnd = strchr(req, ' ');
    if (methodEnd == NULL) {
        free(request);
        return NULL;  // Invalid request format
    }

    size_t methodLength = methodEnd - req;
    char methodString[methodLength + 1];
    strncpy(methodString, req, methodLength);
    methodString[methodLength] = '\0';

    // Map the method string to the Method enum
    if (strcmp(methodString, "GET") == 0) {
        request->method = GET;
    } else if (strcmp(methodString, "POST") == 0) {
        request->method = POST;
    } else if (strcmp(methodString, "HEAD") == 0) {
        request->method = HEAD;
    } else if (strcmp(methodString, "PATCH") == 0) {
        request->method = PATCH;
    } else if (strcmp(methodString, "PUT") == 0) {
        request->method = PUT;
    } else if (strcmp(methodString, "OPTIONS") == 0) {
        request->method = OPTIONS;
    } else {
        free(request);
        return NULL; // Unsupported method
    }

    // Parse the path
    char *pathStart = methodEnd + 1;
    char *protocolStart = strstr(pathStart, " HTTP/");
    if (protocolStart == NULL) {
        free(request);
        return NULL;  // Invalid request format
    }

    size_t pathLength = protocolStart - pathStart;
    request->path = (char *)malloc(pathLength + 1);
    if (request->path == NULL) {
        perror("Memory allocation failed");
        free(request);
        return NULL;
    }

    strncpy(request->path, pathStart, pathLength);
    request->path[pathLength] = '\0';

    // Find the query parameters in the path
    char *queryStart = strchr(request->path, '?');
    if (queryStart != NULL) {
        *queryStart = '\0'; // Null-terminate the path
        queryStart++; // Move past the '?' character

        // Count the number of '&' characters to determine the number of query parameters
        char *param = queryStart;
        while (*param) {
            if (*param == '&') {
                request->queryParamsCount++;
            }
            param++;
        }
        request->queryParamsCount++; // Include the last parameter

        // Allocate memory for the KeyValuePair array
        request->queryParams = (KeyValuePair *)malloc(request->queryParamsCount * sizeof(KeyValuePair));
        if (request->queryParams == NULL) {
            perror("Memory allocation failed");
            free(request->path);
            free(request);
            return NULL;
        }

        // Parse and populate the query parameters
        char *paramToken = strtok(queryStart, "&");
        size_t index = 0;
        while (paramToken != NULL) {
            char *equalSign = strchr(paramToken, '=');
            if (equalSign != NULL) {
                *equalSign = '\0';  // Null-terminate the key
                request->queryParams[index].key = strdup(paramToken);
                request->queryParams[index].value = strdup(equalSign + 1);
                index++;
            }
            paramToken = strtok(NULL, "&");
        }
    } else {
        // No query parameters
        request->queryParams = NULL;
        request->queryParamsCount = 0;
    }

    return request;
}

// Function to get a query parameter value by key
char *getQueryParamValue(const Request *request, const char *key) {
    if (request == NULL || key == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < request->queryParamsCount; i++) {
        if (strcmp(request->queryParams[i].key, key) == 0) {
            return request->queryParams[i].value;
        }
    }

    return NULL; // Key not found
}