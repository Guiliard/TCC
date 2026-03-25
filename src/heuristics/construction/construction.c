#include "construction.h"

void resolve_tsp_with_concorde(problem* prob, solution *sol, double *optval) {
    convert_to_symmetric(prob, sol);
    solve_tsp_with_concorde(sol, optval);
    // print_tour(sol->tour_size, sol->tour);
    convert_to_assymmetric(sol);
    // print_tour(sol->tour_size, sol->tour);
    convert_tour_to_min_cost(prob, sol);
    // print_tour(sol->tour_size, sol->tour);
    calculate_objective_function(prob, sol);
}

solution* grasp(problem* prob, int max_iter, float alpha, double *optval) {
    solution* best = NULL;

    for (int i = 0; i < max_iter; i++) {
        solution* sol = build_initial_solution_grasp(prob, alpha, optval);
        vnd(prob, sol, optval);

        if (best == NULL || sol->total_cost < best->total_cost) {
            if(best) free(best);
            best = sol;
        } else {
            free(sol);
        }
    }

    return best;
}

solution* build_initial_solution_grasp(problem *prob, float alpha, double *optval) {
    solution* sol = allocate_vector(sizeof(solution), 1);

    bool *visited = allocate_vector(sizeof(bool), prob->num_all_cities);
    city *selected = allocate_vector(sizeof(city), prob->num_all_cities);

    float *values = allocate_vector(sizeof(float), prob->num_all_cities);
    int *RCL = allocate_vector(sizeof(int), prob->num_all_cities);

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

    sol->visited_cities = selected;
    sol->num_visited_cities = count;
    sol->prize_goal = total_prize;

    resolve_tsp_with_concorde(prob, sol, optval);

    return sol;
}