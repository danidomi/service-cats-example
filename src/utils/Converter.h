#include "../service/Service.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define a generic struct for JSON conversion
typedef struct {
    void *data; // Pointer to the struct data
    char *(*toJson)(Cat *data); // Function pointer to the JSON conversion function
} JsonConverter;

char *convertToJson(const JsonConverter *converter);
char *toJson(Cat *data);