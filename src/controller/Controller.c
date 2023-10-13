#include "Controller.h"

char *get_path() {
    return "/cats";
}

Response *handle_api(Request *request) {
    switch (request->method) {
        case GET:
            return handle_get(request);
        case POST:
            return handle_post(request);
        default:
            // default
            return handle_get(request);
    }
}

Response *handle_get(Request *request) {
    Cat *cat = NULL;
    if (request->queryParams == NULL) {
        return NULL;
    }
    char *id = get_query_param_value(request, "id");
    if (id == NULL) {
        return NULL;
    }
    cat = get_cat(atoi(id));
    Response *response = malloc(sizeof(Response));
    response->status_code = "200 OK";
    response->headers[0] = "Content-Type: application/json; charset=utf-8";
    response->headersCount = 1;

    // Create a JsonConverter for the Cat struct
    JsonConverter catConverter = {
            .data = cat,
            .to_json = to_json,
    };

    // Convert the Cat struct to JSON using the generic function
    char *json = convert_to_json(&catConverter);
    if (json == NULL) {
        return NULL;
    }

    char jsonData[1000];
    sprintf(jsonData, "{\"data\":%s}", json);
    response->data = strdup(jsonData);
    return response;
}

Response *handle_post(Request *request) {
    Cat *cat = NULL;
    if (request->queryParams == NULL) {
        return NULL;
    }
    char *age = get_query_param_value(request, "age");
    if (age == NULL) {
        return NULL;
    }
    int ageInt = atoi(age);
    char *name = get_query_param_value(request, "name");
    if (name == NULL) {
        return NULL;
    }
    cat = create_cat(ageInt, name);
    Response *response = malloc(sizeof(Response));
    response->status_code = "200 OK";
    response->headers[0] = "Content-Type: application/json; charset=utf-8";
    response->headersCount = 1;

    // Create a JsonConverter for the Cat struct
    JsonConverter catConverter = {
            .data = cat,
            .to_json = to_json,
    };

    // Convert the Cat struct to JSON using the generic function
    char *json = convert_to_json(&catConverter);
    if (json == NULL) {
        return NULL;
    }

    char jsonData[1000];
    sprintf(jsonData, "{\"data\":%s}", json);
    response->data = strdup(jsonData);
    return response;
}
