#include "Error.h"

Error * new(char * message) {
    Error * error;
    error->message = message;
    return error;
}