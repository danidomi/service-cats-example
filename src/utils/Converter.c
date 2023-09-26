#include "Converter.h"

// Function to convert a Cat struct to JSON
char *toJson(Cat *data) {
    Cat *cat = (Cat *)data;
    if (cat == NULL) {
        return NULL;
    }

    char *json = (char *)malloc(256); // Allocate memory for the JSON string
    if (json == NULL) {
        return NULL; // Memory allocation failed
    }

    // Create the JSON string
    sprintf(json, "{\"name\":\"%s\",\"age\":%d}", cat->name, cat->age);

    return json;
}

// Function to convert any struct to JSON using a JsonConverter
char *convertToJson(const JsonConverter *converter) {
    if (converter == NULL || converter->toJson == NULL) {
        return NULL;
    }

    return converter->toJson(converter->data);
}