#include "app_metrics.h"

#include <c-framework-service/metrics/metrics.h>

static MetricCounter *cats_created_total;
static MetricCounter *cats_deleted_total;

void app_metrics_init(void) {
    cats_created_total = metric_counter_register(
        "cats_created_total",
        "Cats created (POST /cats) since process start");
    cats_deleted_total = metric_counter_register(
        "cats_deleted_total",
        "Cats deleted (DELETE /cats/:id) since process start");
}

void app_metrics_cat_created(void) {
    metric_counter_inc(cats_created_total);
}

void app_metrics_cat_deleted(void) {
    metric_counter_inc(cats_deleted_total);
}
