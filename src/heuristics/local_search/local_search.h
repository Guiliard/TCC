#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "../../utils/utils.h"
#include "../../solver/solver.h"
#include "../../convert/min_cost.h"
#include "../../convert/symmetric.h"
#include "../../convert/asymmetric.h"
#include "../../calculate/calculate.h"

int compare_parameter_asc(const void *a, const void *b);
int compare_parameter_desc(const void *a, const void *b);

void shuffle_array(int *array, int size);
void roulette_select(int *candidates, int num_candidates);
void select_candidates(int *candidates, int num_candidates, int selection, int (*compar)(const void*, const void*));

void rebuild_city_pos_in_tour(problem *prob, solution *sol);

float find_best_insertion_position(problem *prob, solution *sol, int selection, int *best_city_id, int *best_position, int *best_dist_delta);
float find_best_drop_position(problem *prob, solution *sol, int selection, int *best_city_id, int *best_position, int *best_dist_delta);
float find_best_swap_position(problem *prob, solution *sol, int selection, int *best_city_in_id, int *best_city_out_id, int *best_position, int *best_new_prize_goal, int *best_dist_delta);
float find_best_two_opt_position(problem *prob, solution *sol, int *best_i, int *best_j);
float find_best_relocate_position(problem *prob, solution *sol, int *best_from, int *best_to);

void insert_city_in_tour(solution *sol, int city_k_id, int position);
void drop_city_from_tour(solution *sol, int position);
void swap_city_in_tour(solution *sol, int city_k_id, int position);
void reverse_tour_segment(solution *sol, int left, int right);
void relocate_city_in_tour(solution *sol, int from_pos, int to_pos);

void insertion_move(problem* prob, solution* sol, int selection);
void drop_move(problem* prob, solution* sol, int selection);
void swap_move(problem* prob, solution* sol, int selection);
void two_opt_move(problem* prob, solution* sol);
void relocate_move(problem* prob, solution* sol);

void vnd(problem* prob, solution* sol, int selection);

#endif