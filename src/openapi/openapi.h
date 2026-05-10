#pragma once

#include <c-framework-service/request/request.h>
#include <c-framework-service/response/response.h>

Response *handle_swagger_ui(Request *request);
Response *handle_openapi_json(Request *request);
