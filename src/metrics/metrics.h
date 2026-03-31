#ifndef METRICS_H
#define METRICS_H

#include "../utils/libs.h"

typedef struct {
    struct timeval start_time;
    double elapsed_time;
} metrics;

metrics* create_metrics();
void start_metrics(metrics* m);
void stop_metrics(metrics* m);
void print_metrics(metrics* m, float reference_time);
void free_metrics(metrics* m);

#endif