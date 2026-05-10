# service-cats-example

A small RESTful microservice in C demonstrating
[c-framework-service](https://github.com/danidomi/c-framework-service) v0.4.0.
Manages a list of cats — list, read, create, update, delete — with interactive
Swagger UI docs and a separate admin port for health probes and Prometheus
metrics.

## API

Main port (`$PORT`, default 8080):

| Method | Path           | Body                       | Notes                              |
|--------|----------------|----------------------------|------------------------------------|
| GET    | `/`            | —                          | Swagger UI                         |
| GET    | `/openapi.json`| —                          | OpenAPI 3.0.3 spec                 |
| GET    | `/cats`        | —                          | List all                           |
| POST   | `/cats`        | `{"name":"X","age":N}`     | id is server-generated             |
| GET    | `/cats/:id`    | —                          | Get one (404 if missing)           |
| PUT    | `/cats/:id`    | `{"name":"X","age":N}`     | Update (404 if missing)            |
| DELETE | `/cats/:id`    | —                          | 204 (404 if missing)               |

Admin port (`$ADMIN_PORT`, default 9090):

| Method | Path        | Notes                                                  |
|--------|-------------|--------------------------------------------------------|
| GET    | `/healthz`  | Liveness — always 200 if the process answers           |
| GET    | `/readyz`   | Readiness — 200 / 503 (currently always 200)           |
| GET    | `/metrics`  | Prometheus text format (built-in counters + custom)    |

## Repository backend

The cat store is selected at compile time:

```shell
make DB=memory       # default — pthread-safe linked list, no external DB
make DB=mysql        # libmysqlclient + the docker-compose db
```

The in-memory backend seeds three cats on startup so the API has something to
list out of the box. It does **not** persist across restarts — fine for demos
and Render's free tier (which spins down after 15 min idle anyway). For
durable storage, swap to MySQL or add a Postgres backend (see the comment
in `Makefile`).

## Build and run locally

```shell
cdeps install                 # fetch c-framework-service v0.4.0
make DB=memory                # build into bin/service-cats-example
PORT=8080 ./bin/service-cats-example
```

Then open http://localhost:8080/ for Swagger UI, or:

```shell
curl http://localhost:8080/cats
curl -X POST -H 'Content-Type: application/json' \
     -d '{"name":"Luna","age":2}' http://localhost:8080/cats
curl http://localhost:9090/metrics
```

## Docker

Default build is the in-memory backend; `docker-compose` overrides to MySQL:

```shell
# in-memory, single image — what Render uses
docker build -t service-cats-example .
docker run --rm -p 8080:8080 -p 9090:9090 service-cats-example

# MySQL stack — local dev
docker compose up
```

## Deploy to Render

1. Push to GitHub.
2. https://dashboard.render.com → **New → Web Service**.
3. Connect the repo. Runtime: **Docker** (auto-detected). Plan: **Free**.
4. No environment variables needed — `DB=memory` is the Dockerfile default and
   the in-memory backend has no external dependencies.
5. Deploy.

Render only publishes one port externally (`$PORT`), so `/metrics` on 9090
stays internal — that's the desired security boundary. If you want metrics
publicly scrapable instead, edit `main.c`: replace
`health_start_admin_server(0)` with `health_register_default_routes()` to
mount the probes on the main port.

## Environment variables

| Variable          | Default | Effect                                                            |
|-------------------|---------|-------------------------------------------------------------------|
| `PORT`            | 8080    | Main API listener                                                 |
| `ADMIN_PORT`      | 9090    | Admin/metrics listener                                            |
| `METRICS_ENABLED` | true    | Set `0`/`false`/`no`/`off` to disable collection and `/metrics`   |
| `MYSQL_HOST`      | 127.0.0.1 | Only when built with `DB=mysql`                                 |
| `MYSQL_PORT`      | 3306    | "                                                                 |
| `MYSQL_USERNAME`  | root    | "                                                                 |
| `MYSQL_PASSWORD`  | root    | "                                                                 |
| `MYSQL_DATABASE`  | cats    | "                                                                 |

## Custom metrics

App-level Prometheus counters live in `src/metrics/app_metrics.{h,c}`. To add
a new metric:

```c
// app_metrics.h
void app_metrics_my_event(void);

// app_metrics.c
static MetricCounter *my_event_total;

void app_metrics_init(void) {
    /* ...existing... */
    my_event_total = metric_counter_register(
        "my_event_total", "What this counts");
}

void app_metrics_my_event(void) {
    metric_counter_inc(my_event_total);
}
```

Then call `app_metrics_my_event()` from wherever the event happens.
`metric_gauge_register` / `_set` / `_inc` / `_dec` follow the same pattern.

## Project layout

```
main.c                       wires routes + admin server + repository init
src/config/                  DbConfig from env
src/repository/              repository_memory.c, repository_mysql.c (selectable)
src/service/                 input validation, calls into repository
src/controller/              HTTP handlers
src/openapi/                 Swagger UI HTML + OpenAPI 3.0.3 spec
src/metrics/app_metrics.c    app-level Prometheus counters
src/utils/json_input.c       small JSON field extractor for request bodies
src/utils/converter.c        Cat -> JSON helper
src/model/cat.{h,c}          Cat struct + to_string
db/init.sql                  MySQL seed (only used by docker-compose)
```
