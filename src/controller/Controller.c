#include "Controller.h"

char *get_path() {
    return "/cat";
}

Response *handle_api(Request *request) {
    switch (request->method) {
        case GET:
            return handleGET(request);
        case POST:
            return handlePOST(request);
        default:
            // default
            return handleGET(request);
    }
}

Response *handleGET(Request *request) {
    Cat *cat = NULL;
    if (request->queryParams == NULL) {
        return NULL;
    }
    char *id = getQueryParamValue(request, "id");
    if (id == NULL) {
        return NULL;
    }
    cat = GetCat(atoi(id));
    Response *response = malloc(sizeof(Response));
    response->status_code = "200 OK";
    response->headers[0] = "Content-Type: application/json; charset=utf-8";
    response->headersCount = 1;

    // Create a JsonConverter for the Cat struct
    JsonConverter catConverter = {
            .data = cat,
            .toJson = toJson,
    };

    // Convert the Cat struct to JSON using the generic function
    char *json = convertToJson(&catConverter);
    if (json == NULL) {
        return NULL;
    }

    char jsonData[1000];
    sprintf(jsonData, "{\"data\":%s}", json);
    response->data = strdup(jsonData);
    return response;
}

Response *handlePOST(Request *request) {
    Cat *cat = NULL;
    if (request->queryParams == NULL) {
        return NULL;
    }
    char *age = getQueryParamValue(request, "age");
    if (age == NULL) {
        return NULL;
    }
    int ageInt = atoi(age);
    char *name = getQueryParamValue(request, "name");
    if (name == NULL) {
        return NULL;
    }
    cat = CreateCat(ageInt, name);
    Response *response = malloc(sizeof(Response));
    response->status_code = "200 OK";
    response->headers[0] = "Content-Type: application/json; charset=utf-8";
    response->headersCount = 1;

    // Create a JsonConverter for the Cat struct
    JsonConverter catConverter = {
            .data = cat,
            .toJson = toJson,
    };

    // Convert the Cat struct to JSON using the generic function
    char *json = convertToJson(&catConverter);
    if (json == NULL) {
        return NULL;
    }

    char jsonData[1000];
    sprintf(jsonData, "{\"data\":%s}", json);
    response->data = strdup(jsonData);
    return response;
}
