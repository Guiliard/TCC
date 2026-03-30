#ifndef UTILS_H
#define UTILS_H

#include "libs.h"

#define max(x,y) ((x)<(y) ? (y) : (x))
#define min(x,y) ((x)>(y) ? (y) : (x))

#define INT_MAX __INT_MAX__
#define FLT_MAX	__FLT_MAX__

#define PERCENT_OF_PRIZE 0.2
#define MAX_ITER 1
#define ALPHA 0.1f

typedef struct city {
	int id;
    int prize;
    int penalty;
    float avg_distance;
    float parameter;
} city;

typedef struct problem {
    city* all_cities;
    int** asymmetric_distances;
    int num_all_cities;
    int total_prize;
    int min_prize_goal;
    float percent_of_prize;
    float alpha;
} problem;

typedef struct solution {
    city* visited_cities;
    int** symmetric_distances;
    int* tour;
    int symmetric_distances_size;
    int tour_size;
    int num_visited_cities;
    int prize_goal;
    float total_cost;
    double tour_cost;
} solution;

void *allocate_vector(size_t element_size, size_t count);
int **allocate_matrix(int num_rows, int num_cols);

void print_matrix(int num_rows, int num_cols, int **matrix);
void print_tour(int tour_size, int *tour);

void print_problem(problem *prob);
void print_solution(solution *sol);

solution* copy_solution(solution *sol);

void free_problem(problem *prob);
void free_solution(solution *sol);

#endif