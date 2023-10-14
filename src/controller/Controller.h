#pragma once
#include <c-framework-service/Server.h>
#include "../service/Service.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/Converter.h"

Response *handle_get(Request *request);
Response *handle_post(Request *request);
