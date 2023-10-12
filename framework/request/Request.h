#include <stdio.h>
#include <stdlib.h>


// Define log levels
typedef enum {
    GET,
    POST,
    HEAD,
    PATCH,
    PUT,
    OPTIONS
} Method;

typedef struct KeyValuePair {
    char *key;
    char *value;
} KeyValuePair;

typedef struct Request {
    Method method;
    char *path;
    KeyValuePair *queryParams;
    size_t queryParamsCount;
} Request;

Request *parse_request(char *req);
char *get_query_param_value(const Request *request, const char *key);