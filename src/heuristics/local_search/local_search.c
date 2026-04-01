#include "local_search.h"

problem *global_prob = NULL;
int global_dist = 0;

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

void shuffle_array(int *array, int size)
{
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void roulette_select(int *candidates, int num_candidates)
{
    int *selected = allocate_vector(sizeof(int), num_candidates);
    bool *used = allocate_vector(sizeof(bool), num_candidates);

    for (int s = 0; s < num_candidates; s++) {
        float random_value = (rand() % 100) + 1;
        float cumulative = 0.0f;
        int selected_index = -1;

        float remaining_parameter = 0.0f;
        for (int i = 0; i < num_candidates; i++) {
            if (!used[i]) {
                remaining_parameter += global_prob->all_cities[candidates[i]].parameter;
            }
        }

        if (remaining_parameter <= 0.0f) {
            for (int i = 0; i < num_candidates; i++) {
                if (!used[i]) {
                    selected_index = i;
                    break;
                }
            }
        } else {
            for (int i = 0; i < num_candidates; i++) {
                if (!used[i]) {
                    float probability = (global_prob->all_cities[candidates[i]].parameter / remaining_parameter) * 100.0f;
                    cumulative += probability;
                    if (random_value <= cumulative) {
                        selected_index = i;
                        break;
                    }
                }
            }

            if (selected_index == -1) {
                for (int i = num_candidates - 1; i >= 0; i--) {
                    if (!used[i]) {
                        selected_index = i;
                        break;
                    }
                }
            }
        }

        selected[s] = candidates[selected_index];
        used[selected_index] = true;
    }

    for (int i = 0; i < num_candidates; i++) {
        candidates[i] = selected[i];
    }

    free(selected);
    free(used);
}

void select_candidates(int *candidates, int num_candidates, int selection, int (*compar)(const void*, const void*))
{
    switch (selection) {
        case CANDIDATE_SELECTION_ORDERED:
            qsort(candidates, num_candidates, sizeof(int), compar);
            break;
        case CANDIDATE_SELECTION_RANDOM:
            shuffle_array(candidates, num_candidates);
            break;
        case CANDIDATE_SELECTION_ROULETTE:
            roulette_select(candidates, num_candidates);
            break;
    }
}

float find_best_insertion_position(problem *prob, solution *sol, int city_k_id, int *best_position)
{
    float best_delta = FLT_MAX;
    *best_position = -1;
    
    for (int i = 0; i < sol->tour_size - 1; i++) {
        int j = i + 1;

        int city_i_id = sol->tour[i];
        int city_j_id = sol->tour[j];
        
        int d_ij = prob->asymmetric_distances[city_i_id][city_j_id];
        int d_ik = prob->asymmetric_distances[city_i_id][city_k_id];
        int d_kj = prob->asymmetric_distances[city_k_id][city_j_id];
        
        int penalty_k = prob->all_cities[city_k_id].penalty;

        int dist_delta = d_ik + d_kj - d_ij;
        float delta = (float)(dist_delta) - (float)penalty_k;
        
        if (delta < best_delta) {
            best_delta = delta;
            global_dist = dist_delta;
            *best_position = i;
        }
    }
    
    return best_delta;
}

float find_best_drop_position(problem *prob, solution *sol, int city_k_id, int *best_position)
{
    *best_position = -1;
    
    for(int i = 1; i < sol->tour_size - 1; i++) {
        if(sol->tour[i] == city_k_id) {
            *best_position = i;
            break;
        }
    }
    
    if(*best_position == -1) {
        return FLT_MAX;
    }
    
    int city_i_id = sol->tour[*best_position - 1];
    int city_j_id = sol->tour[*best_position + 1];
    
    int d_ij = prob->asymmetric_distances[city_i_id][city_j_id];
    int d_ik = prob->asymmetric_distances[city_i_id][city_k_id];
    int d_kj = prob->asymmetric_distances[city_k_id][city_j_id];

    int penalty_k = prob->all_cities[city_k_id].penalty;
    
    int dist_delta = d_ij - d_ik - d_kj;
    float delta = (float)(dist_delta) + (float)penalty_k;
    global_dist = dist_delta;

    return delta;
}

float find_best_swap_position(problem *prob, solution *sol, int city_k_id, int city_r_id, int *best_position)
{
    *best_position = -1;
    
    for(int pos = 1; pos < sol->tour_size - 1; pos++) {
        if(sol->tour[pos] == city_r_id) {
            *best_position = pos;
            break;
        }
    }
    
    if(*best_position == -1) {
        return FLT_MAX;
    }
    
    int city_i_id = sol->tour[*best_position - 1];
    int city_j_id = sol->tour[*best_position + 1];
    
    int penalty_k = prob->all_cities[city_k_id].penalty;
    int penalty_r = prob->all_cities[city_r_id].penalty;
    
    int d_ik = prob->asymmetric_distances[city_i_id][city_k_id];
    int d_kj = prob->asymmetric_distances[city_k_id][city_j_id];
    int d_ir = prob->asymmetric_distances[city_i_id][city_r_id];
    int d_rj = prob->asymmetric_distances[city_r_id][city_j_id];
    
    int dist_delta = d_ik + d_kj - d_ir - d_rj;
    float delta = (float)(dist_delta) + (float)(penalty_r - penalty_k);
    global_dist = dist_delta;

    return delta;
}

void insert_city_in_tour(solution *sol, int city_k_id, int insert_after_pos)
{
    int *new_tour = allocate_vector(sizeof(int), sol->tour_size + 1);

    memcpy(new_tour, sol->tour, (insert_after_pos + 1) * sizeof(int));

    new_tour[insert_after_pos + 1] = city_k_id;

    memcpy(new_tour + insert_after_pos + 2, sol->tour + insert_after_pos + 1,   (sol->tour_size - insert_after_pos - 1) * sizeof(int));
    
    free(sol->tour);

    sol->tour = new_tour;
    sol->tour_size++;
}

void drop_city_from_tour(solution *sol, int position_in_tour)
{
    int *new_tour = allocate_vector(sizeof(int), sol->tour_size - 1);
    
    memcpy(new_tour, sol->tour, position_in_tour * sizeof(int));
    memcpy(new_tour + position_in_tour, sol->tour + position_in_tour + 1, (sol->tour_size - position_in_tour - 1) * sizeof(int));
    
    free(sol->tour);

    sol->tour = new_tour;
    sol->tour_size--;
}

void swap_city_in_tour(solution *sol, int city_k_id, int position_to_replace)
{
    sol->tour[position_to_replace] = city_k_id;
}

void insertion_move(problem *prob, solution *sol, int selection)
{
    bool *in_solution = allocate_vector(sizeof(bool), prob->num_all_cities);

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
    
    select_candidates(candidates, num_candidates, selection, compare_parameter_desc);

    for(int c = 0; c < num_candidates; c++) {
        int city_k_id = candidates[c];

        int best_position = -1;
        float best_delta = find_best_insertion_position(prob, sol, city_k_id, &best_position);

        if(best_delta < 0.0f && best_position >= 0) {
            city *new_visited = realloc(sol->visited_cities, (sol->num_visited_cities + 1) * sizeof(city));
            new_visited[sol->num_visited_cities] = prob->all_cities[city_k_id];
            
            sol->visited_cities = new_visited;
            sol->num_visited_cities++;
            
            insert_city_in_tour(sol, city_k_id, best_position);
            
            sol->prize_goal += prob->all_cities[city_k_id].prize;
            sol->total_cost += best_delta;
            sol->tour_cost += global_dist;
            
            free(in_solution);
            free(candidates);

            global_dist = 0;
            global_prob = NULL;

            return;
        }
    }
    
    free(in_solution);
    free(candidates);
}

void drop_move(problem *prob, solution *sol, int selection)
{
    int *candidates = allocate_vector(sizeof(int), sol->num_visited_cities);
    int num_candidates = 0;

    for(int i = 1; i < sol->num_visited_cities; i++) {
        int new_prize = sol->prize_goal - sol->visited_cities[i].prize;

        if(new_prize >= prob->min_prize_goal) {
            candidates[num_candidates++] = sol->visited_cities[i].id;
        }
    }

    if(num_candidates == 0) {
        free(candidates);
        return;
    }

    global_prob = prob;
    
    select_candidates(candidates, num_candidates, selection, compare_parameter_asc);

    for(int c = 0; c < num_candidates; c++) {
        int city_k_id = candidates[c];

        int best_position = -1;
        float best_delta = find_best_drop_position(prob, sol, city_k_id, &best_position);

        if(best_delta < 0.0f && best_position >= 1) {
            int remove_idx = -1;

            for(int i = 1; i < sol->num_visited_cities; i++) {
                if(sol->visited_cities[i].id == city_k_id) {
                    remove_idx = i;
                    break;
                }
            }

            if (remove_idx == -1) continue;
            
            city *new_visited = allocate_vector(sizeof(city), sol->num_visited_cities - 1);
            
            memcpy(new_visited, sol->visited_cities, remove_idx * sizeof(city));
            memcpy(new_visited + remove_idx, sol->visited_cities + remove_idx + 1, (sol->num_visited_cities - remove_idx - 1) * sizeof(city));
            
            free(sol->visited_cities);

            sol->visited_cities = new_visited;
            sol->num_visited_cities--;
            
            drop_city_from_tour(sol, best_position);
            
            sol->prize_goal -= prob->all_cities[city_k_id].prize;
            sol->total_cost += best_delta;
            sol->tour_cost += global_dist;
            
            free(candidates);

            global_dist = 0;
            global_prob = NULL;

            return;
        }
    }

    free(candidates);
}

void swap_move(problem *prob, solution *sol, int selection)
{
    bool *in_solution = allocate_vector(sizeof(bool), prob->num_all_cities);

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

    int *inside_candidates = allocate_vector(sizeof(int), sol->num_visited_cities - 1);
    int num_inside = 0;

    for(int i = 1; i < sol->num_visited_cities; i++) {
        inside_candidates[num_inside++] = sol->visited_cities[i].id;
    }

    if(num_outside == 0 || num_inside == 0) {
        free(in_solution);
        free(outside_candidates);
        free(inside_candidates);
        return;
    }

    global_prob = prob;

    select_candidates(outside_candidates, num_outside, selection, compare_parameter_desc);
    select_candidates(inside_candidates, num_inside, selection, compare_parameter_asc);

    for(int o = 0; o < num_outside; o++) {
        int city_k_id = outside_candidates[o];
        
        for(int i = 0; i < num_inside; i++) {
            int city_r_id = inside_candidates[i];

            int old_prize = prob->all_cities[city_r_id].prize;
            int new_prize = prob->all_cities[city_k_id].prize;
            int new_prize_goal = sol->prize_goal - old_prize + new_prize;
            
            if(new_prize_goal < prob->min_prize_goal) {
                continue;
            }
            
            int best_position = -1;
            float best_delta = find_best_swap_position(prob, sol, city_k_id, city_r_id, &best_position);

            if(best_delta < 0.0f && best_position >= 1) {
                for (int v = 0; v < sol->num_visited_cities; v++) {
                    if(sol->visited_cities[v].id == city_r_id) {
                        sol->visited_cities[v] = prob->all_cities[city_k_id];
                        break;
                    }
                }
                
                swap_city_in_tour(sol, city_k_id, best_position);
                
                sol->prize_goal = new_prize_goal;
                sol->total_cost += best_delta;
                sol->tour_cost += global_dist;

                free(in_solution);
                free(outside_candidates);
                free(inside_candidates);

                global_dist = 0;
                global_prob = NULL;

                return;
            }
        }
    }

    free(in_solution);
    free(outside_candidates);
    free(inside_candidates);
}

void vnd(problem *prob, solution *sol, int selection)
{
    int k = 1;

    while(k <= 3) {

        float cost_before = sol->total_cost;

        if(k == 1) {
            insertion_move(prob, sol, selection);
        }
        else if(k == 2) {
            swap_move(prob, sol, selection);
        }
        else if(k == 3) {
            drop_move(prob, sol, selection);
        }
        
        if(sol->total_cost < cost_before) {
            k = 1;
        } else {
            k++;
        }
    }
}