#include "local_search.h"

problem *global_prob;

int compare_parameter_asc(const void *a, const void *b)
{
    int city_a = *(int*)a;
    int city_b = *(int*)b;

    float pa = global_prob->all_cities[city_a].parameter;
    float pb = global_prob->all_cities[city_b].parameter;

    if (pa > pb) return 1;
    if (pa < pb) return -1;

    return 0;
}

int compare_parameter_desc(const void *a, const void *b)
{
    int city_a = *(int*)a;
    int city_b = *(int*)b;

    float pa = global_prob->all_cities[city_a].parameter;
    float pb = global_prob->all_cities[city_b].parameter;

    if (pa < pb) return 1;
    if (pa > pb) return -1;

    return 0;
}

bool try_solution(problem *prob, solution *sol, double *optval, float original_cost)
{
    convert_to_symmetric(prob, sol);
    solve_tsp_with_concorde(sol, optval);
    convert_to_asymmetric(sol);
    convert_tour_to_min_cost(prob, sol);
    calculate_objective_function(prob, sol);

    return sol->total_cost < original_cost;
}

void insertion_move(problem *prob, solution *sol, double *optval)
{
    bool *in_solution = allocate_vector(sizeof(bool), prob->num_all_cities);

    for(int i = 0; i < prob->num_all_cities; i++) {
        in_solution[i] = false;
    }

    for(int i = 0; i < sol->num_visited_cities; i++) {
        in_solution[sol->visited_cities[i].id] = true;
    }

    int *candidates = allocate_vector(sizeof(int), prob->num_all_cities - sol->num_visited_cities);
    int num_candidates = 0;

    for(int i = 0; i < prob->num_all_cities; i++) {
        if(!in_solution[i]) {
            candidates[num_candidates++] = i;
        }
    }

    global_prob = prob;

    qsort(candidates, num_candidates, sizeof(int), compare_parameter_desc);

    for(int c = 0; c < num_candidates; c++) {
        int city_id = candidates[c];

        solution *temp_solution = copy_solution(sol);

        city *new_visited = realloc(temp_solution->visited_cities, (temp_solution->num_visited_cities + 1) * sizeof(city));
        if(new_visited == NULL) {
            free_solution(temp_solution);
            continue;
        }
        
        temp_solution->visited_cities = new_visited;
        temp_solution->visited_cities[temp_solution->num_visited_cities] = prob->all_cities[city_id];
        temp_solution->num_visited_cities = temp_solution->num_visited_cities + 1;
        temp_solution->prize_goal += prob->all_cities[city_id].prize;

        if(try_solution(prob, temp_solution, optval, sol->total_cost)) {
            
            free(sol->visited_cities);
            if(sol->symmetric_distances != NULL) {
                for (int i = 0; i < sol->symmetric_distances_size; i++) {
                    free(sol->symmetric_distances[i]);
                }
                free(sol->symmetric_distances);
            }
            free(sol->tour);
            
            sol->visited_cities = temp_solution->visited_cities;
            sol->symmetric_distances = temp_solution->symmetric_distances;
            sol->symmetric_distances_size = temp_solution->symmetric_distances_size;
            sol->tour = temp_solution->tour;
            sol->tour_size = temp_solution->tour_size;
            sol->num_visited_cities = temp_solution->num_visited_cities;
            sol->prize_goal = temp_solution->prize_goal;
            sol->total_cost = temp_solution->total_cost;
            
            free(temp_solution);
            
            free(in_solution);
            free(candidates);
            return;
        }
        
        free_solution(temp_solution);
    }

    free(in_solution);
    free(candidates);
}

void drop_move(problem *prob, solution *sol, double *optval)
{
    if(sol->num_visited_cities <= 1) {
        return;
    }

    int *candidates = allocate_vector(sizeof(int), sol->num_visited_cities);
    int num_candidates = 0;

    int current_prize = sol->prize_goal;

    for(int i = 1; i < sol->num_visited_cities; i++) {
        int new_prize = current_prize - sol->visited_cities[i].prize;

        if(new_prize >= prob->min_prize_goal) {
            candidates[num_candidates++] = sol->visited_cities[i].id;
        }
    }

    if(num_candidates == 0) {
        free(candidates);
        return;
    }

    global_prob = prob;

    qsort(candidates, num_candidates, sizeof(int), compare_parameter_asc);

    for(int c = 0; c < num_candidates; c++) {
        int city_id = candidates[c];
        int remove_index = -1;

        solution *temp_solution = copy_solution(sol);

        for(int k = 1; k < temp_solution->num_visited_cities; k++) {
            if(temp_solution->visited_cities[k].id == city_id) {
                remove_index = k;
                break;
            }
        }

        if(remove_index == -1) {
            free_solution(temp_solution);
            continue;
        }

        city removed = temp_solution->visited_cities[remove_index];

        for(int i = remove_index; i < temp_solution->num_visited_cities - 1; i++) {
            temp_solution->visited_cities[i] = temp_solution->visited_cities[i+1];
        }

        temp_solution->num_visited_cities = temp_solution->num_visited_cities - 1;
        temp_solution->prize_goal -= removed.prize;

        if(try_solution(prob, temp_solution, optval, sol->total_cost)) {
            
            free(sol->visited_cities);
            if(sol->symmetric_distances != NULL) {
                for (int i = 0; i < sol->symmetric_distances_size; i++) {
                    free(sol->symmetric_distances[i]);
                }
                free(sol->symmetric_distances);
            }
            free(sol->tour);
            
            sol->visited_cities = temp_solution->visited_cities;
            sol->symmetric_distances = temp_solution->symmetric_distances;
            sol->symmetric_distances_size = temp_solution->symmetric_distances_size;
            sol->tour = temp_solution->tour;
            sol->tour_size = temp_solution->tour_size;
            sol->num_visited_cities = temp_solution->num_visited_cities;
            sol->prize_goal = temp_solution->prize_goal;
            sol->total_cost = temp_solution->total_cost;
            
            free(temp_solution);
            
            free(candidates);
            return;
        }
        
        free_solution(temp_solution);
    }

    free(candidates);
}

void swap_move(problem *prob, solution *sol, double *optval)
{
    bool *in_solution = allocate_vector(sizeof(bool), prob->num_all_cities);

    for(int i = 0; i < prob->num_all_cities; i++) {
        in_solution[i] = false;
    }

    for(int i = 0; i < sol->num_visited_cities; i++) {
        in_solution[sol->visited_cities[i].id] = true;
    }

    int *outside_candidates = allocate_vector(sizeof(int), prob->num_all_cities - sol->num_visited_cities);
    int num_outside = 0;

    for(int i = 0; i < prob->num_all_cities; i++) {
        if(!in_solution[i]) {
            outside_candidates[num_outside++] = i;
        }
    }

    int *inside_candidates = allocate_vector(sizeof(int), sol->num_visited_cities);
    int num_inside = 0;

    for(int i = 1; i < sol->num_visited_cities; i++) {
        inside_candidates[num_inside++] = sol->visited_cities[i].id;
    }

    global_prob = prob;

    qsort(outside_candidates, num_outside, sizeof(int), compare_parameter_desc);

    qsort(inside_candidates, num_inside, sizeof(int), compare_parameter_asc);

    for(int o = 0; o < num_outside; o++) {

        int outside_id = outside_candidates[o];
        city outside_city = prob->all_cities[outside_id];

        for(int i = 0; i < num_inside; i++) {

            int inside_id = inside_candidates[i];
            int pos = -1;

            for(int k = 1; k < sol->num_visited_cities; k++) {
                if(sol->visited_cities[k].id == inside_id) {
                    pos = k;
                    break;
                }
            }

            if(pos == -1) continue;

            city inside_city = sol->visited_cities[pos];

            int new_prize = sol->prize_goal - inside_city.prize + outside_city.prize;

            if(new_prize < prob->min_prize_goal) continue;

            solution *temp_solution = copy_solution(sol);
            temp_solution->visited_cities[pos] = outside_city;
            temp_solution->prize_goal = new_prize;

            if(try_solution(prob, temp_solution, optval, sol->total_cost)) {
                
                free(sol->visited_cities);
                if(sol->symmetric_distances != NULL) {
                    for (int i = 0; i < sol->symmetric_distances_size; i++) {
                        free(sol->symmetric_distances[i]);
                    }
                    free(sol->symmetric_distances);
                }
                free(sol->tour);
                
                sol->visited_cities = temp_solution->visited_cities;
                sol->symmetric_distances = temp_solution->symmetric_distances;
                sol->symmetric_distances_size = temp_solution->symmetric_distances_size;
                sol->tour = temp_solution->tour;
                sol->tour_size = temp_solution->tour_size;
                sol->num_visited_cities = temp_solution->num_visited_cities;
                sol->prize_goal = temp_solution->prize_goal;
                sol->total_cost = temp_solution->total_cost;
                
                free(temp_solution);
                
                free(in_solution);
                free(outside_candidates);
                free(inside_candidates);
                return;
            }
            
            free_solution(temp_solution);
        }
    }

    free(in_solution);
    free(outside_candidates);
    free(inside_candidates);
}

void vnd(problem *prob, solution *sol, double *optval)
{
    int k = 1;

    while(k <= 3) {

        float cost_before = sol->total_cost;

        if(k == 1) {
            insertion_move(prob, sol, optval);
        }
        else if(k == 2) {
            swap_move(prob, sol, optval);
        }
        else if(k == 3) {
            drop_move(prob, sol, optval);
        }
        
        if(sol->total_cost < cost_before) {
            k = 1;
        } else {
            k++;
        }
    }
}