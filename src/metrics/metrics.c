#include "metrics.h"

metrics* create_metrics()
{
    metrics* m = (metrics*)malloc(sizeof(metrics));
    m->elapsed_time = 0.0;
    return m;
}

void start_metrics(metrics* m)
{
    gettimeofday(&m->start_time, NULL);
}

void stop_metrics(metrics* m)
{
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    
    m->elapsed_time = (end_time.tv_sec - m->start_time.tv_sec) + (end_time.tv_usec - m->start_time.tv_usec) / 1000000.0;
}

void print_metrics(metrics* m, float reference_time)
{
    printf("\n--- Métricas de Desempenho ---\n");
    printf("Tempo de execução: %.6f segundos\n", m->elapsed_time);
    printf("Tempo de referência: %.6f segundos\n", reference_time);
    float ratio = reference_time / m->elapsed_time;
    printf("Comparação: %.2f vezes mais rápido\n", ratio);
}

void free_metrics(metrics* m)
{
    free(m);
}