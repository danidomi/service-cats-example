#include "converter.h"

#include "../model/cat.h"

/* Returns a malloc'd string. Caller frees. */
char *to_json(Cat *data) {
    if (!data) return NULL;
    return to_string(data);
}

char *convert_to_json(const JsonConverter *converter) {
    if (!converter || !converter->to_json) return NULL;
    return converter->to_json(converter->data);
}
