#include <c-framework-service/router/router.h>
#include <c-framework-service/server/server.h>

#include "src/controller/controller.h"

int main(void) {
    route_register(GET,  "/cats", handle_get_cat);
    route_register(POST, "/cats", handle_post_cat);
    return server_run(PORT);
}
