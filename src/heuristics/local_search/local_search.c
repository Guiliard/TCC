#include "local_search.h"

void swap_int(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sort_candidates_by_parameter(problem *prob, int *candidates, int num_candidates, int descending)
{
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = i + 1; j < num_candidates; j++) {
            float pi = prob->all_cities[candidates[i]].parameter;
            float pj = prob->all_cities[candidates[j]].parameter;

            int should_swap = descending ? (pj > pi) : (pj < pi);

            if (should_swap) {
                int temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }
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

void roulette_select(problem *prob, int *candidates, int num_candidates)
{
    int *selected = allocate_vector(sizeof(int), num_candidates);
    bool *used = allocate_vector(sizeof(bool), num_candidates);

    for (int s = 0; s < num_candidates; s++) {
        float remaining_parameter = 0.0f;

        for (int i = 0; i < num_candidates; i++) {
            if (!used[i]) {
                remaining_parameter += prob->all_cities[candidates[i]].parameter;
            }
        }

        if (remaining_parameter <= 0.0f) {
            shuffle_array(candidates, num_candidates);
            free(selected);
            free(used);
            return;
        }

        float random_value = ((float) rand() / RAND_MAX) * remaining_parameter;

        float cumulative = 0.0f;
        int selected_index = -1;

        for (int i = 0; i < num_candidates; i++) {
            if (used[i]) {
                continue;
            }

            cumulative += prob->all_cities[candidates[i]].parameter;

            if (cumulative >= random_value) {
                selected_index = i;
                break;
            }
        }

        if (selected_index == -1) {
            for (int i = 0; i < num_candidates; i++) {
                if (!used[i]) {
                    selected_index = i;
                    break;
                }
            }
        }

        selected[s] = candidates[selected_index];
        used[selected_index] = true;
    }

    memcpy(candidates, selected, num_candidates * sizeof(int));

    free(selected);
    free(used);
}

void select_candidates(problem *prob, int *candidates, int num_candidates, int selection, int descending)
{
    switch (selection) {
        case CANDIDATE_SELECTION_ORDERED:
            sort_candidates_by_parameter(prob, candidates, num_candidates, descending);
            break;

        case CANDIDATE_SELECTION_RANDOM:
            shuffle_array(candidates, num_candidates);
            break;

        case CANDIDATE_SELECTION_ROULETTE:
            roulette_select(prob, candidates, num_candidates);
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

    select_candidates(prob, candidates, num_candidates, selection, 1);

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

    select_candidates(prob, candidates, num_candidates, selection, 0);

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

    select_candidates(prob, outside_candidates, num_outside, selection, 1);
    select_candidates(prob, inside_candidates, num_inside, selection, 0);

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
        
        calculate_objective_function(prob, sol);
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

        calculate_objective_function(prob, sol);
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

        calculate_objective_function(prob, sol);
    }
}

void two_opt_move(problem *prob, solution *sol)
{
    int best_i = -1;
    int best_j = -1;

    float best_delta = find_best_two_opt_position(prob, sol, &best_i, &best_j);

    if (best_delta < 0.0f && best_i >= 1 && best_j >= best_i) {
        reverse_tour_segment(sol, best_i, best_j);
        
        rebuild_city_pos_in_tour(prob, sol);

        calculate_objective_function(prob, sol);
    }
}

void relocate_move(problem *prob, solution *sol)
{
    int best_from = -1;
    int best_to = -1;

    float best_delta = find_best_relocate_position(prob, sol, &best_from, &best_to);

    if (best_delta < 0.0f && best_from >= 1 && best_to >= 0) {
        relocate_city_in_tour(sol, best_from, best_to);

        rebuild_city_pos_in_tour(prob, sol);

        calculate_objective_function(prob, sol);
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

void generate_orders_recursive(int *base, int left, int right, int orders[NUM_VND_ORDERS][5], int *count)
{
    if (left == right) {
        memcpy(orders[*count], base, 5 * sizeof(int));
        (*count)++;
        return;
    }

    for (int i = left; i <= right; i++) {
        swap_int(&base[left], &base[i]);
        generate_orders_recursive(base, left + 1, right, orders, count);
        swap_int(&base[left], &base[i]);
    }
}

solution* local_search_sequential_all_orders(problem *prob, solution *sol, int selection)
{
    int orders[NUM_VND_ORDERS][5];

    int base_order[5] = {
        MOVE_INSERTION,
        MOVE_SWAP,
        MOVE_TWO_OPT,
        MOVE_RELOCATE,
        MOVE_DROP
    };

    int order_count = 0;
    generate_orders_recursive(base_order, 0, 4, orders, &order_count);

    solution *improved_sol = copy_solution(prob, sol);

    for (int i = 0; i < order_count; i++) {
        vnd(prob, improved_sol, selection, orders[i]);
    }

    return improved_sol;
}

solution* local_search_sequential_best_order(problem *prob, solution *sol, int selection)
{
    int orders[NUM_VND_ORDERS][5];

    int base_order[5] = {
        MOVE_INSERTION,
        MOVE_SWAP,
        MOVE_TWO_OPT,
        MOVE_RELOCATE,
        MOVE_DROP
    };

    int order_count = 0;
    generate_orders_recursive(base_order, 0, 4, orders, &order_count);

    solution *best_sol = NULL;
    float best_cost = FLT_MAX;

    for (int i = 0; i < order_count; i++) {
        solution *test_sol = copy_solution(prob, sol);

        vnd(prob, test_sol, selection, orders[i]);

        if (test_sol->total_cost < best_cost) {
            if (best_sol != NULL) {
                free_solution(best_sol);
            }

            best_cost = test_sol->total_cost;
            best_sol = test_sol;
        } else {
            free_solution(test_sol);
        }
    }

    return best_sol;
}