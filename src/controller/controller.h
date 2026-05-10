#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <c-framework-service/request/request.h>
#include <c-framework-service/response/response.h>

#include "../service/service.h"
#include "../utils/converter.h"

Response *handle_get_cat(Request *request);
Response *handle_post_cat(Request *request);
Response *handle_list_cats(Request *request);
Response *handle_put_cat(Request *request);
Response *handle_delete_cat(Request *request);
