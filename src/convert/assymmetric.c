#include "assymmetric.h"

void convert_to_assymmetric(solution *sol) {
    printf("Convertendo tour simétrico de volta para o problema original...\n");

    int start_index = -1;

    for (int i = 0; i < sol->tour_size; i++) {
        if (sol->tour[i] == 0) {
            start_index = i;
            break;
        }
    }
    
    if (start_index == -1) {
        printf("ERRO: Não foi encontrado um nó de partida no tour!\n");
        return;
    }
    
    int *original_tour = (int *)allocate_vector(sizeof(int), sol->num_visited_cities + 1);
    bool *visited = (bool *)allocate_vector(sizeof(bool), sol->num_visited_cities);
    int original_count = 0;
    
    for (int i = 0; i < sol->tour_size; i++) {
        int current_index = (start_index + i) % sol->tour_size;
        int node = sol->tour[current_index];
        
        int solution_index;
        if (node < sol->num_visited_cities) {
            solution_index = node;
        } else {
            solution_index = node - sol->num_visited_cities;
        }
        
        if (solution_index < sol->num_visited_cities && !visited[solution_index]) {
            int original_city_index = sol->visited_cities[solution_index].id;
            original_tour[original_count++] = original_city_index;
            visited[solution_index] = true;

            printf("Node simétrico %d -> Solução index %d -> Cidade original %d\n", node, solution_index, original_city_index);
            
            if (original_count >= sol->num_visited_cities) {
                break;
            }
        }
    }

    original_tour[original_count] = original_tour[0];
    
    free(visited);
    free(sol->tour);

    sol->tour = original_tour;
    sol->tour_size = original_count + 1;
}