#pragma once

/* App-level metrics. Call app_metrics_init() once at startup, then
 * use the increment helpers from anywhere — they no-op safely when
 * METRICS_ENABLED=0 in the environment. */

void app_metrics_init(void);
void app_metrics_cat_created(void);
void app_metrics_cat_deleted(void);
