#include "converter.h"

// Function to convert a Cat struct to JSON
char *to_json(Cat *data) {
    Cat *cat = (Cat *)data;
    if (cat == NULL) {
        return NULL;
    }

    char *json = (char *)malloc(256); // Allocate memory for the JSON string
    if (json == NULL) {
        return NULL; // Memory allocation failed
    }

    // Create the JSON string
    sprintf(json, "%s", to_string(cat));

    return json;
}

// Function to convert any struct to JSON using a JsonConverter
char *convert_to_json(const JsonConverter *converter) {
    if (converter == NULL || converter->to_json == NULL) {
        return NULL;
    }

    return converter->to_json(converter->data);
}