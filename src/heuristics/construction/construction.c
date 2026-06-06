#include "construction.h"

solution* grasp(problem *prob, int max_iter, float alpha, int candidate_selection_strategy)
{
    int local_search_threads = get_num_threads();

    solution *thread_best_sol[local_search_threads];
    float thread_best_cost[local_search_threads];

    for (int t = 0; t < local_search_threads; t++) {
        thread_best_sol[t] = NULL;
        thread_best_cost[t] = FLT_MAX;
    }

    #pragma omp parallel num_threads(local_search_threads)
    {
        int tid = omp_get_thread_num();

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < max_iter; i++) {
            int solver_type = (i % 2 == 0) ? SOLVER_NEAREST : SOLVER_CHEAPEST;

            solution* current_solution = build_initial_solution_grasp(prob, alpha, solver_type);
            solution* ls_current_solution = local_search_sequential_best_order(prob, current_solution, candidate_selection_strategy);

            free_solution(current_solution);

            if (ls_current_solution->total_cost < thread_best_cost[tid]) {
                if (thread_best_sol[tid] != NULL) {
                    free_solution(thread_best_sol[tid]);
                }

                thread_best_cost[tid] = ls_current_solution->total_cost;
                thread_best_sol[tid] = ls_current_solution;
            } else {
                free_solution(ls_current_solution);
            }
        }
    }

    solution *best_sol = NULL;
    float best_cost = FLT_MAX;

    for (int t = 0; t < local_search_threads; t++) {
        if (thread_best_sol[t] != NULL && thread_best_cost[t] < best_cost) {
            if (best_sol != NULL) {
                free_solution(best_sol);
            }

            best_cost = thread_best_cost[t];
            best_sol = thread_best_sol[t];
            thread_best_sol[t] = NULL;
        }
    }

    for (int t = 0; t < local_search_threads; t++) {
        if (thread_best_sol[t] != NULL) {
            free_solution(thread_best_sol[t]);
        }
    }

    if (best_sol != NULL) {
        double old_tour_cost = best_sol->tour_cost;
        float old_total_cost = best_sol->total_cost;
        int old_tour_size = best_sol->tour_size;

        int *old_tour = allocate_vector(sizeof(int), old_tour_size);
        memcpy(old_tour, best_sol->tour, old_tour_size * sizeof(int));

        convert_to_symmetric(prob, best_sol);

        if (solve_tsp_with_concorde(best_sol)) {
            convert_to_asymmetric(best_sol);
            convert_tour_to_min_cost(prob, best_sol);
            calculate_objective_function(prob, best_sol);

            if (best_sol->total_cost >= old_total_cost) {
                free(best_sol->tour);

                best_sol->tour = old_tour;
                best_sol->tour_size = old_tour_size;
                best_sol->tour_cost = old_tour_cost;
                best_sol->total_cost = old_total_cost;
                
                rebuild_city_pos_in_tour(prob, best_sol);
            } else {
                free(old_tour);
            }
        } else {
            free(old_tour);
        }
    }

    return best_sol;
}

solution* build_initial_solution_grasp(problem *prob, float alpha, int solver_type)
{
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
        float best = -FLT_MAX;
        float worst = FLT_MAX;

        for (int i = 0; i < prob->num_all_cities; i++) {
            values[i] = -FLT_MAX;

            if (!visited[i]) {
                float p = prob->all_cities[i].parameter;

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
    sol->total_cost = 0.0;

    if (solver_type == SOLVER_NEAREST) {
        solve_tsp_with_nearest_neighbor(prob, sol);
    } else {
        solve_tsp_with_cheapest_insertion(prob, sol);
    }

    calculate_objective_function(prob, sol);

    return sol;
}