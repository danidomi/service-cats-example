#include "controller.h"

#include <c-framework-service/error/error.h>

static Response *error_response(const char *status, const char *message) {
    Response *r = response_new(status);
    if (!r) return NULL;
    response_add_header(r, "Content-Type: application/json; charset=utf-8");
    char body[512];
    snprintf(body, sizeof(body), "{\"error\":\"%s\"}", message ? message : "unknown");
    response_set_body(r, body);
    return r;
}

static Response *cat_response(const char *status, Cat *cat) {
    Response *r = response_new(status);
    if (!r) return NULL;
    response_add_header(r, "Content-Type: application/json; charset=utf-8");

    JsonConverter conv = {.data = cat, .to_json = to_json};
    char *json = convert_to_json(&conv);
    if (!json) {
        response_free(r);
        return NULL;
    }

    size_t need = strlen(json) + 16;
    char *body = malloc(need);
    if (!body) {
        free(json);
        response_free(r);
        return NULL;
    }
    snprintf(body, need, "{\"data\":%s}", json);
    response_set_body(r, body);
    free(body);
    free(json);
    return r;
}

Response *handle_get_cat(Request *request) {
    const char *id = get_query_param_value(request, "id");
    if (!id) return error_response("400 Bad Request", "missing 'id' query param");

    Error *err = NULL;
    Cat *cat = get_cat(atoi(id), &err);

    if (err) {
        Response *r = error_response("500 Internal Server Error", err->message);
        error_free(err);
        return r;
    }
    if (!cat) {
        return error_response("404 Not Found", "cat not found");
    }

    return cat_response("200 OK", cat);
}

Response *handle_post_cat(Request *request) {
    const char *age = get_query_param_value(request, "age");
    const char *name = get_query_param_value(request, "name");
    if (!age || !name) return error_response("400 Bad Request", "missing 'age' or 'name'");

    Error *err = NULL;
    Cat *cat = create_cat(atoi(age), (char *)name, &err);

    if (err) {
        const char *status = cat ? "500 Internal Server Error" : "400 Bad Request";
        Response *r = error_response(status, err->message);
        error_free(err);
        return r;
    }
    if (!cat) {
        return error_response("400 Bad Request", "invalid input");
    }

    return cat_response("201 Created", cat);
}
