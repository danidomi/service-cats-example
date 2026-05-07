#include <c-framework-service/router/router.h>
#include <c-framework-service/server/server.h>
#include <c-framework-service/logger/logger.h>

#include "src/config/config.h"
#include "src/controller/controller.h"
#include "src/repository/repository.h"

int main(void) {
    DbConfig cfg = db_config_from_env();
    log_message(INFO, "db target: %s:%d/%s (user=%s)",
                cfg.host, cfg.port, cfg.database, cfg.user);

    repository_init(&cfg);
    db_config_free(&cfg);

    route_register(GET,  "/cats", handle_get_cat);
    route_register(POST, "/cats", handle_post_cat);

    int rc = server_run(port_from_env(PORT));
    repository_close();
    return rc;
}
