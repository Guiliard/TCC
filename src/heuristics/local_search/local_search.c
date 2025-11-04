#include "local_search.h"

void local_search_insertion(int **tour, unsigned int *tour_size, int **distances, city *cities, unsigned int num_cities, float *fo) {
   
    bool *visited = (bool *)allocate_vector(sizeof(bool), num_cities);
    for (unsigned int i = 0; i < *tour_size; i++) {
        visited[(*tour)[i]] = true;
    }

    float best_fo = *fo;
    int *best_tour = NULL;
    unsigned int best_tour_size = *tour_size;
    bool improved = false;

    for (unsigned int city_idx = 0; city_idx < num_cities; city_idx++) {
        if (visited[city_idx] || city_idx == 0) continue;

        printf("Testando cidade %u (penalidade: %u)\n", city_idx, cities[city_idx].penalty);

        for (unsigned int pos = 1; pos <= *tour_size; pos++) {
            
            unsigned int new_tour_size = *tour_size + 1;
            int *new_tour = (int *)allocate_vector(sizeof(int), new_tour_size);
            
            new_tour[0] = (*tour)[0];

            for (unsigned int i = 1; i < pos; i++) {
                new_tour[i] = (*tour)[i];
            }
            
            new_tour[pos] = (int)city_idx;
            
            for (unsigned int i = pos; i < *tour_size; i++) {
                new_tour[i + 1] = (*tour)[i];
            }

            double new_fo = calculate_objective_function(new_tour, new_tour_size, distances, cities, num_cities);

            if (new_fo < best_fo) {
                printf("  *** MELHORIA ENCONTRADA! ***\n");
                best_fo = new_fo;
                if (best_tour) free(best_tour);
                best_tour = new_tour;
                best_tour_size = new_tour_size;
                improved = true;
            } else {
                free(new_tour);
            }
        }
    }

    if (improved) {
        free(*tour);
        *tour = best_tour;
        *tour_size = best_tour_size;
        *fo = best_fo;
        printf("\nBusca local melhorou FO para: %.2f\n", best_fo);
        printf("Novo tour (%u cidades): ", best_tour_size);
        for (unsigned int i = 0; i < best_tour_size; i++) {
            printf("%d ", best_tour[i]);
        }
        printf("\n");
    } else {
        printf("\nBusca local não encontrou melhorias\n");
    }

    free(visited);
}

void local_search_iterative(int **tour, unsigned int *tour_size, int **distances, city *cities, unsigned int num_cities, float *fo, unsigned int max_iterations) {
    
    unsigned int iteration = 0;
    float previous_fo = *fo;
    
    while (iteration < max_iterations) {
        printf("\n=== Iteração %u ===\n", iteration + 1);
        
        local_search_insertion(tour, tour_size, distances, cities, num_cities, fo);
        
        if (*fo >= previous_fo) {
            printf("Não há mais melhorias - parando.\n");
            break;
        }
        
        previous_fo = *fo;
        iteration++;
    }
}