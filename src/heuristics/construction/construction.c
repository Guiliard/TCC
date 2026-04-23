#include "construction.h"

solution* grasp(problem* prob, int max_iter, float alpha, int candidate_selection_strategy) {
    solution* best_sol = NULL;

    for (int i = 0; i < max_iter; i++) {
        solution* current_sol = build_initial_solution_grasp(prob, alpha);
        vnd(prob, current_sol, candidate_selection_strategy);

        if (best_sol == NULL || current_sol->total_cost < best_sol->total_cost) {
            if (best_sol != NULL) {
                free_solution(best_sol);
            }
            best_sol = current_sol;
        } else {
            free_solution(current_sol);
        }
    }

    return best_sol;
}

solution* build_initial_solution_grasp(problem *prob, float alpha) {
    solution* sol = allocate_vector(sizeof(solution), 1);

    bool *visited = allocate_vector(sizeof(bool), prob->num_all_cities);
    city *selected = allocate_vector(sizeof(city), prob->num_all_cities);

    float *values = allocate_vector(sizeof(float), prob->num_all_cities);
    int *RCL = allocate_vector(sizeof(int), prob->num_all_cities);

    int* positions_in_visited = allocate_vector(sizeof(int), prob->num_all_cities);

    int count = 1;
    int total_prize = 0;
    int capacity = prob->min_prize_goal;

    selected[0] = prob->all_cities[0];
    visited[0] = true;

    while (total_prize < capacity && count < prob->num_all_cities) {
        int last_city = selected[count-1].id;
        
        float best = -FLT_MAX;
        float worst = FLT_MAX;

        for (int i = 0; i < prob->num_all_cities; i++) {
            values[i] = -FLT_MAX;

            if (!visited[i]) {
                //float p = prob->all_cities[i].parameter;
                float p = calculate_greedy_value(prob, last_city, i);

                values[i] = p;

                if (p > best) best = p;
                if (p < worst) worst = p;
            }
        }

        float threshold = best - alpha * (best - worst);

        int rcl_size = 0;

        for (int i = 0; i < prob->num_all_cities; i++) {
            if (!visited[i] && values[i] >= threshold) {
                RCL[rcl_size++] = i;
            }
        }

        int selected_index = -1;

        if (rcl_size > 0) {
            selected_index = RCL[rand() % rcl_size];
        } else {
            for (int i = 0; i < prob->num_all_cities; i++) {
                if (!visited[i]) {
                    selected_index = i;
                    break;
                }
            }
        }

        selected[count] = prob->all_cities[selected_index];
        visited[selected_index] = true;

        total_prize += prob->all_cities[selected_index].prize;

        count++;
    }

    free(values);
    free(RCL);
    free(visited);

    selected = realloc(selected, count * sizeof(city));

    for(int i = 0; i < prob->num_all_cities; i++) {
        positions_in_visited[i] = -1;
    }

    for(int i = 0; i < count; i++) {
        int city_id = selected[i].id;
        positions_in_visited[city_id] = i;
    }

    sol->visited_cities = selected;
    sol->num_visited_cities = count;
    sol->prize_goal = total_prize;
    sol->symmetric_distances = NULL;
    sol->symmetric_distances_size = 0;
    sol->tour = NULL;
    sol->tour_size = 0;
    sol->tour_cost = 0.0;
    sol->city_pos_in_visited = positions_in_visited;
    sol->city_pos_in_tour = NULL;

    convert_to_symmetric(prob, sol);
    solve_tsp_with_concorde(sol);
    convert_to_asymmetric(sol);
    convert_tour_to_min_cost(prob, sol);
    calculate_objective_function(prob, sol);

    return sol;
}