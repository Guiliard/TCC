#ifndef UTILS_H
#define UTILS_H

#include "libs.h"

#define max(x,y) ((x)<(y) ? (y) : (x))
#define min(x,y) ((x)>(y) ? (y) : (x))

#define INT_MAX __INT_MAX__
#define FLT_MAX	__FLT_MAX__

#define INSTANCE_NAME_SIZE 128

#define PERCENT_OF_PRIZE 0.2

#define MAX_ITER 100

#define ALPHA 0.1f

#define CANDIDATE_SELECTION_ORDERED     0
#define CANDIDATE_SELECTION_RANDOM      1
#define CANDIDATE_SELECTION_ROULETTE    2

#define SOLVER_NEAREST   0
#define SOLVER_CHEAPEST  1

#define MOVE_INSERTION 1
#define MOVE_SWAP      2
#define MOVE_TWO_OPT   3
#define MOVE_RELOCATE  4
#define MOVE_DROP      5

#define TIME_LIMIT_CONCORDE 60.0

#define LOCAL_SEARCH_THREADS 6
#define NUM_VND_ORDERS 120

typedef struct city {
	int id;

    int prize;
    int penalty;

    float avg_distance;
    float parameter;
} city;

typedef struct problem {
    city* all_cities;
    int num_all_cities;

    int** asymmetric_distances;

    int total_prize;
    float percent_of_prize;
    int min_prize_goal;

    float alpha;
} problem;

typedef struct solution {
    city* visited_cities;
    int num_visited_cities;

    int** symmetric_distances;
    int symmetric_distances_size;

    int *city_pos_in_tour;
    int *city_pos_in_visited;

    int* tour;
    int tour_size;
    double tour_cost;

    int prize_goal;

    float total_cost;
} solution;

void *allocate_vector(size_t element_size, size_t count);
int **allocate_matrix(int num_rows, int num_cols);

void print_matrix(int num_rows, int num_cols, int **matrix);
void print_tour(int tour_size, int *tour);

void print_problem(problem *prob);
void print_solution(solution *sol);

void free_problem(problem *prob);
void free_solution(solution *sol);

solution *copy_solution(problem *prob, solution *src);

void print_report(const char *instance_name, problem *prob, solution *sol, double execution_time, float alpha, int num_iterations, int candidate_selection_type);

#endif