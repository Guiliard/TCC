#include "local_search.h"

problem *global_prob = NULL;

void swap_int(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

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

    for(int s = 0; s < num_candidates; s++) {

        float remaining_parameter = 0.0f;

        for(int i = 0; i < num_candidates; i++) {
            if(!used[i]) {
                remaining_parameter += global_prob->all_cities[candidates[i]].parameter;
            }
        }

        if (remaining_parameter <= 0.0f) {
            shuffle_array(candidates, num_candidates);

            free(selected);
            free(used);

            return;
        }

        float random_value = ((float)rand() / RAND_MAX) * remaining_parameter;

        float cumulative = 0.0f;
        int selected_index = -1;

        for(int i = 0; i < num_candidates; i++) {

            if(used[i]) continue;

            cumulative += global_prob->all_cities[candidates[i]].parameter;

            if(cumulative >= random_value) {
                selected_index = i;
                break;
            }
        }

        if(selected_index == -1) {
            for(int i = 0; i < num_candidates; i++) {
                if(!used[i]) {
                    selected_index = i;
                    break;
                }
            }
        }

        selected[s] = candidates[selected_index];
        used[selected_index] = true;
    }

    memcpy(candidates, selected, num_candidates*sizeof(int));

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

void rebuild_city_pos_in_tour(problem *prob, solution *sol)
{
    for (int i = 0; i < prob->num_all_cities; i++) {
        sol->city_pos_in_tour[i] = -1;
    }

    for (int i = 0; i < sol->tour_size - 1; i++) {
        sol->city_pos_in_tour[sol->tour[i]] = i;
    }
}

float find_best_insertion_position(problem *prob, solution *sol, int selection, int *best_city_id, int *best_position, int *best_dist_delta) 
{
    *best_city_id = -1;
    *best_position = -1;
    *best_dist_delta = 0;

    float best_delta = 0.0f;

    int *candidates = allocate_vector(sizeof(int), prob->num_all_cities - sol->num_visited_cities);
    int num_candidates = 0;

    for (int i = 0; i < prob->num_all_cities; i++) {
        if (sol->city_pos_in_visited[i] == -1) {
            candidates[num_candidates++] = i;
        }
    }

    if (num_candidates == 0) {
        free(candidates);
        return best_delta;
    }

    global_prob = prob;
    select_candidates(candidates, num_candidates, selection, compare_parameter_desc);

    int **dist = prob->asymmetric_distances;
    int *tour = sol->tour;

    for (int c = 0; c < num_candidates; c++) {
        int city_k_id = candidates[c];
        int penalty_k = prob->all_cities[city_k_id].penalty;

        for (int pos = 0; pos < sol->tour_size - 1; pos++) {
            int city_i = tour[pos];
            int city_j = tour[pos + 1];

            int dist_delta = dist[city_i][city_k_id] + dist[city_k_id][city_j] - dist[city_i][city_j];

            float delta = (float)dist_delta - penalty_k;

            if (delta < best_delta) {
                best_delta = delta;
                *best_city_id = city_k_id;
                *best_position = pos;
                *best_dist_delta = dist_delta;
            }
        }
    }

    free(candidates);
    global_prob = NULL;

    return best_delta;
}

float find_best_drop_position(problem *prob, solution *sol, int selection, int *best_city_id, int *best_position, int *best_dist_delta) 
{
    *best_city_id = -1;
    *best_position = -1;
    *best_dist_delta = 0;

    float best_delta = 0.0f;

    int *candidates = allocate_vector(sizeof(int), sol->num_visited_cities);
    int num_candidates = 0;

    for (int i = 1; i < sol->num_visited_cities; i++) {
        int city_id = sol->visited_cities[i].id;
        int new_prize = sol->prize_goal - sol->visited_cities[i].prize;

        if (new_prize >= prob->min_prize_goal) {
            candidates[num_candidates++] = city_id;
        }
    }

    if (num_candidates == 0) {
        free(candidates);
        return best_delta;
    }

    global_prob = prob;
    select_candidates(candidates, num_candidates, selection, compare_parameter_asc);

    int **dist = prob->asymmetric_distances;
    int *tour = sol->tour;

    for (int c = 0; c < num_candidates; c++) {
        int city_k_id = candidates[c];
        int pos = sol->city_pos_in_tour[city_k_id];

        if (pos <= 0 || pos >= sol->tour_size - 1) {
            continue;
        }

        int city_i = tour[pos - 1];
        int city_j = tour[pos + 1];

        int dist_delta =
            dist[city_i][city_j] -
            dist[city_i][city_k_id] -
            dist[city_k_id][city_j];

        int penalty_k = prob->all_cities[city_k_id].penalty;

        float delta = (float)dist_delta + penalty_k;

        if (delta < best_delta) {
            best_delta = delta;
            *best_city_id = city_k_id;
            *best_position = pos;
            *best_dist_delta = dist_delta;
        }
    }

    free(candidates);
    global_prob = NULL;

    return best_delta;
}

float find_best_swap_position(problem *prob, solution *sol, int selection, int *best_city_in_id, int *best_city_out_id, int *best_position, int *best_new_prize_goal, int *best_dist_delta) 
{
    *best_city_in_id = -1;
    *best_city_out_id = -1;
    *best_position = -1;
    *best_new_prize_goal = sol->prize_goal;
    *best_dist_delta = 0;

    float best_delta = 0.0f;

    int *outside_candidates = allocate_vector(sizeof(int), prob->num_all_cities - sol->num_visited_cities);
    int num_outside = 0;

    for (int i = 0; i < prob->num_all_cities; i++) {
        if (sol->city_pos_in_visited[i] == -1) {
            outside_candidates[num_outside++] = i;
        }
    }

    int *inside_candidates = allocate_vector(sizeof(int), sol->num_visited_cities - 1);
    int num_inside = 0;

    for (int i = 1; i < sol->num_visited_cities; i++) {
        inside_candidates[num_inside++] = sol->visited_cities[i].id;
    }

    if (num_outside == 0 || num_inside == 0) {
        free(outside_candidates);
        free(inside_candidates);
        return best_delta;
    }

    global_prob = prob;

    select_candidates(outside_candidates, num_outside, selection, compare_parameter_desc);
    select_candidates(inside_candidates, num_inside, selection, compare_parameter_asc);

    int **dist = prob->asymmetric_distances;
    int *tour = sol->tour;

    for (int o = 0; o < num_outside; o++) {
        int city_k_id = outside_candidates[o];
        int prize_k = prob->all_cities[city_k_id].prize;
        int penalty_k = prob->all_cities[city_k_id].penalty;

        for (int i = 0; i < num_inside; i++) {
            int city_r_id = inside_candidates[i];
            int prize_r = prob->all_cities[city_r_id].prize;
            int penalty_r = prob->all_cities[city_r_id].penalty;

            int new_prize_goal = sol->prize_goal - prize_r + prize_k;

            if (new_prize_goal < prob->min_prize_goal) {
                continue;
            }

            int pos = sol->city_pos_in_tour[city_r_id];

            if (pos <= 0 || pos >= sol->tour_size - 1) {
                continue;
            }

            int city_i = tour[pos - 1];
            int city_j = tour[pos + 1];

            int dist_delta =
                dist[city_i][city_k_id] +
                dist[city_k_id][city_j] -
                dist[city_i][city_r_id] -
                dist[city_r_id][city_j];

            float delta = (float)dist_delta + (penalty_r - penalty_k);

            if (delta < best_delta) {
                best_delta = delta;
                *best_city_in_id = city_k_id;
                *best_city_out_id = city_r_id;
                *best_position = pos;
                *best_new_prize_goal = new_prize_goal;
                *best_dist_delta = dist_delta;
            }
        }
    }

    free(outside_candidates);
    free(inside_candidates);
    global_prob = NULL;

    return best_delta;
}

float find_best_two_opt_position(problem *prob, solution *sol, int *best_i, int *best_j)
{
    float best_delta = 0.0f;

    *best_i = -1;
    *best_j = -1;

    int *tour = sol->tour;
    int **dist = prob->asymmetric_distances;

    for (int left = 1; left < sol->tour_size - 2; left++) {
        for (int right = left + 1; right < sol->tour_size - 1; right++) {
            double old_cost = 0.0;
            double new_cost = 0.0;

            int before = left - 1;
            int after = right + 1;

            old_cost += dist[tour[before]][tour[left]];

            for (int i = left; i < right; i++) {
                old_cost += dist[tour[i]][tour[i + 1]];
            }

            old_cost += dist[tour[right]][tour[after]];

            new_cost += dist[tour[before]][tour[right]];

            for (int i = right; i > left; i--) {
                new_cost += dist[tour[i]][tour[i - 1]];
            }

            new_cost += dist[tour[left]][tour[after]];

            float delta = (float)(new_cost - old_cost);

            if (delta < best_delta) {
                best_delta = delta;
                *best_i = left;
                *best_j = right;
            }
        }
    }

    return best_delta;
}

float find_best_relocate_position(problem *prob, solution *sol, int *best_from, int *best_to)
{
    float best_delta = 0.0f;

    *best_from = -1;
    *best_to = -1;

    int *tour = sol->tour;
    int **dist = prob->asymmetric_distances;

    for (int from = 1; from < sol->tour_size - 1; from++) {
        int city = tour[from];

        int prev = tour[from - 1];
        int next = tour[from + 1];

        float remove_delta =
            dist[prev][next]
            - dist[prev][city]
            - dist[city][next];

        for (int to = 0; to < sol->tour_size - 1; to++) {
            if (to == from || to == from - 1) {
                continue;
            }

            int insert_after = tour[to];
            int insert_before = tour[to + 1];

            float insert_delta =
                dist[insert_after][city]
                + dist[city][insert_before]
                - dist[insert_after][insert_before];

            float delta = remove_delta + insert_delta;

            if (delta < best_delta) {
                best_delta = delta;
                *best_from = from;
                *best_to = to;
            }
        }
    }

    return best_delta;
}

void insert_city_in_tour(solution *sol, int city_k_id, int insert_after_pos)
{
    sol->tour = realloc(sol->tour, (sol->tour_size + 1) * sizeof(int));

    memmove(
        sol->tour + insert_after_pos + 2,
        sol->tour + insert_after_pos + 1,
        (sol->tour_size - insert_after_pos - 1) * sizeof(int)
    );

    sol->tour[insert_after_pos + 1] = city_k_id;

    sol->tour_size++;
}

void drop_city_from_tour(solution *sol, int position_in_tour)
{
    memmove(
        sol->tour + position_in_tour,
        sol->tour + position_in_tour + 1,
        (sol->tour_size - position_in_tour - 1) * sizeof(int)
    );

    sol->tour_size--;
}

void swap_city_in_tour(solution *sol, int city_k_id, int position_to_replace)
{
    sol->tour[position_to_replace] = city_k_id;
}

void reverse_tour_segment(solution *sol, int left, int right)
{
    while (left < right) {
        int tmp = sol->tour[left];
        sol->tour[left] = sol->tour[right];
        sol->tour[right] = tmp;

        left++;
        right--;
    }
}

void relocate_city_in_tour(solution *sol, int from, int to)
{
    int city = sol->tour[from];

    memmove(
        sol->tour + from,
        sol->tour + from + 1,
        (sol->tour_size - from - 1) * sizeof(int)
    );

    if (to > from) {
        to--;
    }

    memmove(
        sol->tour + to + 2,
        sol->tour + to + 1,
        (sol->tour_size - to - 2) * sizeof(int)
    );

    sol->tour[to + 1] = city;
}

void insertion_move(problem *prob, solution *sol, int selection)
{
    int best_city_id = -1;
    int best_position = -1;
    int best_dist_delta = 0;

    float best_delta = find_best_insertion_position(
        prob,
        sol,
        selection,
        &best_city_id,
        &best_position,
        &best_dist_delta
    );

    if (best_delta < 0.0f && best_city_id != -1 && best_position >= 0) {
        city selected_city = prob->all_cities[best_city_id];

        city *tmp = realloc(sol->visited_cities, (sol->num_visited_cities + 1) * sizeof(city));

        sol->visited_cities = tmp;
        sol->visited_cities[sol->num_visited_cities] = selected_city;
        sol->num_visited_cities++;

        sol->city_pos_in_visited[best_city_id] = sol->num_visited_cities - 1;

        insert_city_in_tour(sol, best_city_id, best_position);
        rebuild_city_pos_in_tour(prob, sol);

        sol->prize_goal += selected_city.prize;
        sol->total_cost += best_delta;
        sol->tour_cost += best_dist_delta;
    }
}

void drop_move(problem *prob, solution *sol, int selection)
{
    int best_city_id = -1;
    int best_position = -1;
    int best_dist_delta = 0;

    float best_delta = find_best_drop_position(
        prob,
        sol,
        selection,
        &best_city_id,
        &best_position,
        &best_dist_delta
    );

    if (best_delta < 0.0f && best_city_id != -1 && best_position >= 1) {
        int remove_idx = sol->city_pos_in_visited[best_city_id];

        if (remove_idx == -1) {
            return;
        }

        city removed_city = prob->all_cities[best_city_id];

        memmove(
            sol->visited_cities + remove_idx,
            sol->visited_cities + remove_idx + 1,
            (sol->num_visited_cities - remove_idx - 1) * sizeof(city)
        );

        sol->num_visited_cities--;

        for (int i = remove_idx; i < sol->num_visited_cities; i++) {
            sol->city_pos_in_visited[sol->visited_cities[i].id] = i;
        }

        sol->city_pos_in_visited[best_city_id] = -1;

        drop_city_from_tour(sol, best_position);
        rebuild_city_pos_in_tour(prob, sol);

        sol->prize_goal -= removed_city.prize;
        sol->total_cost += best_delta;
        sol->tour_cost += best_dist_delta;
    }
}

void swap_move(problem *prob, solution *sol, int selection)
{
    int best_city_in_id = -1;
    int best_city_out_id = -1;
    int best_position = -1;
    int best_new_prize_goal = sol->prize_goal;
    int best_dist_delta = 0;

    float best_delta = find_best_swap_position(
        prob,
        sol,
        selection,
        &best_city_in_id,
        &best_city_out_id,
        &best_position,
        &best_new_prize_goal,
        &best_dist_delta
    );

    if (best_delta < 0.0f && best_city_in_id != -1 && best_city_out_id != -1) {
        int pos_v = sol->city_pos_in_visited[best_city_out_id];

        if (pos_v == -1) {
            return;
        }

        sol->visited_cities[pos_v] = prob->all_cities[best_city_in_id];

        sol->city_pos_in_visited[best_city_out_id] = -1;
        sol->city_pos_in_visited[best_city_in_id] = pos_v;

        swap_city_in_tour(sol, best_city_in_id, best_position);
        rebuild_city_pos_in_tour(prob, sol);

        sol->prize_goal = best_new_prize_goal;
        sol->total_cost += best_delta;
        sol->tour_cost += best_dist_delta;
    }
}

void two_opt_move(problem *prob, solution *sol)
{
    int best_i = -1;
    int best_j = -1;

    float best_delta = find_best_two_opt_position(prob, sol, &best_i, &best_j);

    if (best_delta < 0.0f && best_i >= 1 && best_j >= best_i) {
        reverse_tour_segment(sol, best_i, best_j);

        sol->tour_cost += best_delta;
        sol->total_cost += best_delta;
        
        rebuild_city_pos_in_tour(prob, sol);
    }
}

void relocate_move(problem *prob, solution *sol)
{
    int best_from = -1;
    int best_to = -1;

    float best_delta = find_best_relocate_position(prob, sol, &best_from, &best_to);

    if (best_delta < 0.0f && best_from >= 1 && best_to >= 0) {
        relocate_city_in_tour(sol, best_from, best_to);

        sol->tour_cost += best_delta;
        sol->total_cost += best_delta;

        rebuild_city_pos_in_tour(prob, sol);
    }
}

void vnd(problem *prob, solution *sol, int selection, int *order)
{
    int k = 0;

    while (k < 5) {
        float cost_before = sol->total_cost;

        if (order[k] == MOVE_INSERTION) {
            insertion_move(prob, sol, selection);
        }
        else if (order[k] == MOVE_SWAP) {
            swap_move(prob, sol, selection);
        }
        else if (order[k] == MOVE_TWO_OPT) {
            two_opt_move(prob, sol);
        }
        else if (order[k] == MOVE_RELOCATE) {
            relocate_move(prob, sol);
        }
        else if (order[k] == MOVE_DROP) {
            drop_move(prob, sol, selection);
        }

        if (sol->total_cost < cost_before) {
            k = 0;
        } else {
            k++;
        }
    }
}

solution *permutations_recursive(problem *prob, solution *base_sol, int selection, int *order, int left, int right, float *best_cost, int *best_order, solution *best_sol) 
{
    if (left == right) {
        solution *test_sol = copy_solution(prob, base_sol);

        vnd(prob, test_sol, selection, order);

        if (test_sol->total_cost < *best_cost) {
            *best_cost = test_sol->total_cost;
            memcpy(best_order, order, 5 * sizeof(int));

            if (best_sol != NULL) {
                free_solution(best_sol);
            }

            best_sol = test_sol;
        } else {
            free_solution(test_sol);
        }

        return best_sol;
    }

    for (int i = left; i <= right; i++) {
        swap_int(&order[left], &order[i]);

        best_sol = permutations_recursive(prob, base_sol, selection, order, left + 1, right, best_cost, best_order, best_sol);

        swap_int(&order[left], &order[i]);
    }

    return best_sol;
}

solution* local_search(problem *prob, solution *sol, int selection)
{
    int order[5] = {MOVE_INSERTION, MOVE_SWAP, MOVE_TWO_OPT, MOVE_RELOCATE, MOVE_DROP};

    int best_order[5];
    float best_cost = FLT_MAX;
    solution *best_sol = NULL;

    best_sol = permutations_recursive(prob, sol, selection, order, 0, 4, &best_cost, best_order, best_sol);

    return best_sol;
}