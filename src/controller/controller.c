#include "controller.h"

#include <c-framework-service/error/error.h>

#include "../utils/json_input.h"

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
    if (!id) return error_response(HTTP_BAD_REQUEST, "missing 'id' query param");

    Error *err = NULL;
    Cat *cat = get_cat(atoi(id), &err);

    if (err) {
        Response *r = error_response(HTTP_INTERNAL_SERVER_ERROR, err->message);
        error_free(err);
        return r;
    }
    if (!cat) {
        return error_response(HTTP_NOT_FOUND, "cat not found");
    }

    return cat_response(HTTP_OK, cat);
}

Response *handle_post_cat(Request *request) {
    char *name = json_extract_string(request->body, "name");
    int age = 0;
    int has_age = json_extract_int(request->body, "age", &age);
    if (!name || !has_age) {
        free(name);
        return error_response(HTTP_BAD_REQUEST, "body must be JSON {\\\"name\\\":<string>,\\\"age\\\":<int>}");
    }

    Error *err = NULL;
    Cat *cat = create_cat(age, name, &err);
    free(name);

    if (err) {
        const char *status = cat ? HTTP_INTERNAL_SERVER_ERROR : HTTP_BAD_REQUEST;
        Response *r = error_response(status, err->message);
        error_free(err);
        return r;
    }
    if (!cat) {
        return error_response(HTTP_BAD_REQUEST, "invalid input");
    }

    return cat_response(HTTP_CREATED, cat);
}

Response *handle_list_cats(Request *request) {
    (void)request;

    Error *err = NULL;
    size_t count = 0;
    Cat **cats = list_all_cats(&count, &err);

    if (err) {
        Response *r = error_response(HTTP_INTERNAL_SERVER_ERROR, err->message);
        error_free(err);
        return r;
    }

    Response *r = response_new(HTTP_OK);
    if (!r) {
        cats_free(cats, count);
        return NULL;
    }
    response_add_header(r, "Content-Type: application/json; charset=utf-8");

    /* Build {"data":[item, item, ...]} by concatenating each cat's JSON. */
    size_t cap = 32;
    for (size_t i = 0; i < count; i++) {
        cap += (cats[i]->name ? strlen(cats[i]->name) : 0) + 64;
    }
    char *body = malloc(cap);
    if (!body) {
        cats_free(cats, count);
        response_free(r);
        return NULL;
    }
    size_t pos = 0;
    pos += snprintf(body + pos, cap - pos, "{\"data\":[");
    for (size_t i = 0; i < count; i++) {
        char *one = to_string(cats[i]);
        if (!one) continue;
        pos += snprintf(body + pos, cap - pos, "%s%s", i == 0 ? "" : ",", one);
        free(one);
    }
    pos += snprintf(body + pos, cap - pos, "]}");

    response_set_body(r, body);
    free(body);
    cats_free(cats, count);
    return r;
}

Response *handle_put_cat(Request *request) {
    const char *id = get_query_param_value(request, "id");
    if (!id) return error_response(HTTP_BAD_REQUEST, "missing 'id'");

    char *name = json_extract_string(request->body, "name");
    int age = 0;
    int has_age = json_extract_int(request->body, "age", &age);
    if (!name || !has_age) {
        free(name);
        return error_response(HTTP_BAD_REQUEST, "body must be JSON {\\\"name\\\":<string>,\\\"age\\\":<int>}");
    }

    Error *err = NULL;
    Cat *cat = modify_cat(atoi(id), age, name, &err);
    free(name);

    if (err) {
        Response *r = error_response(HTTP_BAD_REQUEST, err->message);
        error_free(err);
        return r;
    }
    if (!cat) {
        return error_response(HTTP_NOT_FOUND, "cat not found");
    }
    return cat_response(HTTP_OK, cat);
}

Response *handle_delete_cat(Request *request) {
    const char *id = get_query_param_value(request, "id");
    if (!id) return error_response(HTTP_BAD_REQUEST, "missing 'id'");

    Error *err = NULL;
    int rc = remove_cat(atoi(id), &err);

    if (rc < 0) {
        Response *r = error_response(HTTP_INTERNAL_SERVER_ERROR,
                                     err ? err->message : "delete failed");
        if (err) error_free(err);
        return r;
    }
    if (rc == 0) {
        return error_response(HTTP_NOT_FOUND, "cat not found");
    }

    Response *r = response_new(HTTP_NO_CONTENT);
    if (!r) return NULL;
    return r;
}
