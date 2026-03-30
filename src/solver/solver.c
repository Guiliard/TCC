#include "solver.h"

void solve_tsp_with_concorde(solution *sol) {
    CCdatagroup dat;
    CCrandstate rstate;

    int num_nodes = 2 * sol->num_visited_cities;
    int success, foundtour;
    int hit_timebound = 0;
    int silent = 0;

    int *tour_concorde = (int *)allocate_vector(sizeof(int), num_nodes);

    char *name = "my_tsp";
    
    double timebound = 3600.0;

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

        exit(1);
    }

    CCutil_sprand(42, &rstate);
    
    int result = CCtsp_solve_dat(num_nodes, &dat, NULL, tour_concorde, NULL, &sol->tour_cost, &success, &foundtour, name, &timebound, &hit_timebound, silent, &rstate);

    if (result == 0 && foundtour) {
        printf("SOLUÇÃO ENCONTRADA!\n");
        printf("Valor ótimo: %.2f\n", sol->tour_cost);
    } else {
        printf("Não foi possível encontrar uma solução ótima.\n");
        printf("Success: %d, Foundtour: %d\n", success, foundtour);
    }
    
    free(adj);
    free(elist);
    free(elen);

    sol->tour = tour_concorde;
    sol->tour_size = num_nodes;
}