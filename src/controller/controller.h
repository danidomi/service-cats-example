#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <c-framework-service/server.h>

#include "../service/service.h"
#include "../utils/converter.h"

Response *handle_get(Request *request);
Response *handle_post(Request *request);
