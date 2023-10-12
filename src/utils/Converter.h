#include "../service/Service.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define a generic struct for JSON conversion
typedef struct {
    void *data; // Pointer to the struct data
    char *(*to_json)(Cat *data); // Function pointer to the JSON conversion function
} JsonConverter;

char *convert_to_json(const JsonConverter *converter);
char *to_json(Cat *data);