#include "json_input.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Locate the value position for `key` in a flat JSON object. Returns a
 * pointer into `body` just past the colon and any whitespace, or NULL
 * if the key is not found. Naive: matches `"key"` anywhere, so a key
 * appearing inside a string value could be mis-matched. Acceptable
 * for the small, well-known shapes this service deals in. */
static const char *find_key(const char *body, const char *key) {
    if (!body || !key) return NULL;
    size_t klen = strlen(key);
    const char *p = body;
    while ((p = strchr(p, '"')) != NULL) {
        const char *q = p + 1;
        if (strncmp(q, key, klen) == 0 && q[klen] == '"') {
            const char *end = q + klen + 1;
            while (*end && isspace((unsigned char)*end)) end++;
            if (*end == ':') {
                end++;
                while (*end && isspace((unsigned char)*end)) end++;
                return end;
            }
        }
        p++;
    }
    return NULL;
}

char *json_extract_string(const char *body, const char *key) {
    const char *p = find_key(body, key);
    if (!p || *p != '"') return NULL;
    p++;
    const char *start = p;
    while (*p && *p != '"') {
        if (*p == '\\' && *(p + 1)) p++;
        p++;
    }
    if (*p != '"') return NULL;
    size_t len = (size_t)(p - start);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, start, len);
    out[len] = '\0';
    return out;
}

int json_extract_int(const char *body, const char *key, int *out) {
    const char *p = find_key(body, key);
    if (!p) return 0;
    if (*p != '-' && !isdigit((unsigned char)*p)) return 0;
    char *end = NULL;
    long v = strtol(p, &end, 10);
    if (end == p) return 0;
    if (out) *out = (int)v;
    return 1;
}
