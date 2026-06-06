#include "solver.h"

int *build_initial_symmetric_tour_from_solution(solution *sol)
{
    int n = sol->num_visited_cities;
    int num_nodes = 2 * n;

    int *initial_tour = allocate_vector(sizeof(int), num_nodes);

    int idx = 0;

    for (int t = 0; t < sol->tour_size - 1; t++) {
        int current_city = sol->tour[t];
        int next_city = sol->tour[t + 1];

        int current_pos = sol->city_pos_in_visited[current_city];
        int next_pos = sol->city_pos_in_visited[next_city];

        if (current_pos < 0 || current_pos >= n || next_pos < 0 || next_pos >= n) {
            free(initial_tour);
            return NULL;
        }

        initial_tour[idx++] = current_pos;
        initial_tour[idx++] = next_pos + n;
    }

    if (idx != num_nodes) {
        free(initial_tour);
        return NULL;
    }

    return initial_tour;
}

int solve_tsp_with_concorde(solution *sol) {
    CCdatagroup dat;
    CCrandstate rstate;

    double optval;

    int num_nodes = 2 * sol->num_visited_cities;
    int success, foundtour;
    int hit_timebound = 0;
    int silent = 1;

    int *tour_concorde = (int *)allocate_vector(sizeof(int), num_nodes);
    int *initial_tour = build_initial_symmetric_tour_from_solution(sol);

    char *name = "my_tsp";
    
    double timebound = TIME_LIMIT_CONCORDE;

    CCutil_init_datagroup(&dat);
    
    int *adj = (int *)allocate_vector(sizeof(int), num_nodes * num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            adj[i * num_nodes + j] = (i == j) ? 0 : sol->symmetric_distances[i][j];
        }
    }

    int max_edges = num_nodes * (num_nodes - 1) / 2;
    int *elist = (int *)allocate_vector(sizeof(int), 2 * max_edges);
    int *elen = (int *)allocate_vector(sizeof(int), max_edges);
    int ecount = 0;
    
    for (int i = 0; i < num_nodes; i++) {
        for (int j = i + 1; j < num_nodes; j++) {
            elist[2 * ecount] = i;
            elist[2 * ecount + 1] = j;
            elen[ecount] = adj[i * num_nodes + j];
            ecount++;
        }
    }

    if (CCutil_graph2dat_matrix(num_nodes, ecount, elist, elen, 0, &dat) != 0) {
        fprintf(stderr, "Erro na conversão da matriz para CCdatagroup\n");
        free(adj);
        free(elist);
        free(elen);
        free(tour_concorde);
        free(initial_tour);

        return 0;
    }

    CCutil_sprand(42, &rstate);

    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    
    int result = CCtsp_solve_dat(num_nodes, &dat, initial_tour, tour_concorde, NULL, &optval, &success, &foundtour, name, &timebound, &hit_timebound, silent, &rstate);

    fflush(stdout);
    fflush(stderr);

    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);

    close(devnull);
    close(saved_stdout);
    close(saved_stderr);

    if (result != 0 || !foundtour) {
        free(adj);
        free(elist);
        free(elen);
        free(tour_concorde);
        free(initial_tour);
        return 0;
    }
    
    free(adj);
    free(elist);
    free(elen);
    free(initial_tour);

    free(sol->tour);
    sol->tour = tour_concorde;
    sol->tour_size = num_nodes;

    return 1;
}

void solve_tsp_with_nearest_neighbor(problem *prob, solution *sol) {
    int n = sol->num_visited_cities;

    bool *used = allocate_vector(sizeof(bool), n);
    int *tour = allocate_vector(sizeof(int), n + 1);
    int *positions_in_tour = allocate_vector(sizeof(int), prob->num_all_cities);

    for (int i = 0; i < prob->num_all_cities; i++) {
        positions_in_tour[i] = -1;
    }

    int current_pos = 0;
    used[current_pos] = true;
    tour[0] = sol->visited_cities[current_pos].id;
    positions_in_tour[tour[0]] = 0;

    for (int k = 1; k < n; k++) {
        int current_city = sol->visited_cities[current_pos].id;

        int best_pos = -1;
        int best_dist = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!used[i]) {
                int candidate_city = sol->visited_cities[i].id;
                int dist = prob->asymmetric_distances[current_city][candidate_city];

                if (dist < best_dist) {
                    best_dist = dist;
                    best_pos = i;
                }
            }
        }

        if (best_pos == -1) {
            break;
        }

        used[best_pos] = true;
        current_pos = best_pos;
        tour[k] = sol->visited_cities[current_pos].id;
        positions_in_tour[tour[k]] = k;
    }

    tour[n] = tour[0];

    sol->tour = tour;
    sol->tour_size = n + 1;
    sol->city_pos_in_tour = positions_in_tour;

    free(used);
}

void solve_tsp_with_cheapest_insertion(problem *prob, solution *sol) {
    int n = sol->num_visited_cities;

    bool *inserted = allocate_vector(sizeof(bool), n);
    int *tour = allocate_vector(sizeof(int), n + 1);
    int *positions_in_tour = allocate_vector(sizeof(int), prob->num_all_cities);

    for (int i = 0; i < prob->num_all_cities; i++) {
        positions_in_tour[i] = -1;
    }

    tour[0] = sol->visited_cities[0].id;
    inserted[0] = true;
    int second_pos = 1;
    tour[1] = sol->visited_cities[second_pos].id;
    tour[2] = tour[0];
    inserted[second_pos] = true;
    int tour_size = 3;

    while (tour_size < n + 1) {
        int best_city_pos = -1;
        int best_insert_after = -1;
        int best_delta = INT_MAX;

        for (int city_pos = 0; city_pos < n; city_pos++) {
            if (inserted[city_pos]) {
                continue;
            }

            int city_k = sol->visited_cities[city_pos].id;

            for (int pos = 0; pos < tour_size - 1; pos++) {
                int city_i = tour[pos];
                int city_j = tour[pos + 1];

                int delta = prob->asymmetric_distances[city_i][city_k] + prob->asymmetric_distances[city_k][city_j] - prob->asymmetric_distances[city_i][city_j];

                if (delta < best_delta) {
                    best_delta = delta;
                    best_city_pos = city_pos;
                    best_insert_after = pos;
                }
            }
        }

        if (best_city_pos == -1 || best_insert_after == -1) {
            break;
        }

        memmove(
            tour + best_insert_after + 2,
            tour + best_insert_after + 1,
            (tour_size - best_insert_after - 1) * sizeof(int)
        );

        tour[best_insert_after + 1] = sol->visited_cities[best_city_pos].id;
        inserted[best_city_pos] = true;
        tour_size++;
    }

    for (int i = 0; i < tour_size; i++) {
        positions_in_tour[tour[i]] = i;
    }

    positions_in_tour[tour[0]] = 0;

    sol->tour = tour;
    sol->tour_size = tour_size;
    sol->city_pos_in_tour = positions_in_tour;

    free(inserted);
}