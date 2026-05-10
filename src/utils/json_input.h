#pragma once

/* Minimal JSON field extractor for flat top-level objects like
 * {"name":"Luna","age":2}. Not a general-purpose JSON parser — it
 * does not handle nested objects, arrays, or unicode escapes.
 *
 * json_extract_string: returns malloc'd value (caller frees) or NULL
 * if the key is absent or the value is not a string.
 *
 * json_extract_int: returns 1 on success and writes the parsed value
 * to *out; returns 0 if the key is absent or the value is not a
 * number. */
char *json_extract_string(const char *body, const char *key);
int   json_extract_int(const char *body, const char *key, int *out);
