#include <stdlib.h>
#define MAX_HEADERS 20

typedef struct Response {
    char *status_code;
    char *headers[MAX_HEADERS];
    size_t headersCount;
    char *data;
} Response;