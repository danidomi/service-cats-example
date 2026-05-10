#include <c-framework-service/health/health.h>
#include <c-framework-service/logger/logger.h>
#include <c-framework-service/router/router.h>
#include <c-framework-service/server/server.h>

#include "src/config/config.h"
#include "src/controller/controller.h"
#include "src/metrics/app_metrics.h"
#include "src/openapi/openapi.h"
#include "src/repository/repository.h"

int main(void) {
    DbConfig cfg = db_config_from_env();
    log_message(INFO, "db target: %s:%d/%s (user=%s)",
                cfg.host, cfg.port, cfg.database, cfg.user);

    repository_init(&cfg);
    db_config_free(&cfg);
    app_metrics_init();

    route_register(GET,    "/",             handle_swagger_ui);
    route_register(GET,    "/openapi.json", handle_openapi_json);
    route_register(GET,    "/cats",         handle_list_cats);
    route_register(POST,   "/cats",         handle_post_cat);
    route_register(GET,    "/cats/:id",     handle_get_cat);
    route_register(PUT,    "/cats/:id",     handle_put_cat);
    route_register(DELETE, "/cats/:id",     handle_delete_cat);

    /* /healthz, /readyz, /metrics on $ADMIN_PORT (default 9090). */
    health_start_admin_server(0);

    int rc = server_run(port_from_env(PORT));
    repository_close();
    return rc;
}
