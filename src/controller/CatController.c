#include "CatController.h"
#include "../service/Service.h"
#include <stdlib.h>

Response * handleApi(Request * request) {
    Response * response =  malloc(sizeof(Response));
    response->status_code = "200 OK";
    response->content_type = "application/json; charset=utf-8";
    response->response = Create(1,2);
    return response;
}

