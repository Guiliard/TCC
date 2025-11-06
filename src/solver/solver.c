#include "solver.h"

int* solve_tsp_with_concorde(int **symmetric_distances, unsigned int num_nodes, double *optval) {
    CCdatagroup dat;
    int *tour = (int *)allocate_vector(sizeof(int), num_nodes);
    int success, foundtour;
    char *name = "my_tsp";
    double timebound = 3600.0;
    int hit_timebound = 0;
    int silent = 0;
    CCrandstate rstate;

    CCutil_init_datagroup(&dat);
    
    int *adj = (int *)allocate_vector(sizeof(int), num_nodes * num_nodes);
    for (unsigned int i = 0; i < num_nodes; i++) {
        for (unsigned int j = 0; j < num_nodes; j++) {
            adj[i * num_nodes + j] = (i == j) ? 0 : symmetric_distances[i][j];
        }
    }

    int max_edges = num_nodes * (num_nodes - 1) / 2;
    int *elist = (int *)allocate_vector(sizeof(int), 2 * max_edges);
    int *elen = (int *)allocate_vector(sizeof(int), max_edges);
    int ecount = 0;
    
    for (unsigned int i = 0; i < num_nodes; i++) {
        for (unsigned int j = i + 1; j < num_nodes; j++) {
            elist[2 * ecount] = i;
            elist[2 * ecount + 1] = j;
            elen[ecount] = adj[i * num_nodes + j];
            ecount++;
        }
    }

    if (CCutil_graph2dat_matrix((int)num_nodes, ecount, elist, elen, 0, &dat) != 0) {
        fprintf(stderr, "Erro na conversão da matriz para CCdatagroup\n");
        free(adj);
        free(elist);
        free(elen);

        exit(1);
    }

    CCutil_sprand(42, &rstate);
    
    int result = CCtsp_solve_dat((int)num_nodes, &dat, NULL, tour, NULL, optval, &success, &foundtour, name, &timebound, &hit_timebound, silent, &rstate);

    if (result == 0 && foundtour) {
        printf("SOLUÇÃO ENCONTRADA!\n");
        printf("Valor ótimo: %.2f\n", *optval);
    } else {
        printf("Não foi possível encontrar uma solução ótima.\n");
        printf("Success: %d, Foundtour: %d\n", success, foundtour);
    }
    
    free(adj);
    free(elist);
    free(elen);

    return tour;
}