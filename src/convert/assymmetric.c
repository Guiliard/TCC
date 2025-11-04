#include "assymmetric.h"

int* convert_to_assymmetric(int *tour, unsigned int tour_size, unsigned int num_cities, city *initial_solution) {
    printf("Convertendo tour simétrico de volta para o problema original...\n");
    
    int start_index = -1;
    for (unsigned int i = 0; i < tour_size; i++) {
        if ((unsigned int)tour[i] < num_cities) {
            start_index = i;
            break;
        }
    }
    
    if (start_index == -1) {
        printf("ERRO: Não foi encontrado um nó de partida no tour!\n");
        return NULL;
    }
    
    int *original_tour = (int *)allocate_vector(sizeof(int), num_cities);
    bool *visited = (bool *)allocate_vector(sizeof(bool), num_cities);
    int original_count = 0;
    
    for (unsigned int i = 0; i < tour_size; i++) {
        unsigned int current_index = (start_index + i) % tour_size;
        int node = tour[current_index];
        
        unsigned int solution_index;
        if ((unsigned int)node < num_cities) {
            solution_index = node;
        } else {
            solution_index = node - num_cities;
        }
        
        if (!visited[solution_index]) {
            int original_city_index = initial_solution[solution_index].index_city;
            original_tour[original_count++] = original_city_index;
            visited[solution_index] = true;
            
            printf("Nó %d -> Posição %u na solução -> Cidade original %d\n", 
                   node, solution_index, original_city_index);
            
            if ((unsigned int)original_count >= num_cities) {
                break;
            }
        }
    }
    
    printf("Tour com índices originais: ");
    for (int i = 0; i < original_count; i++) {
        printf("%d", original_tour[i]);
        if (i < original_count - 1) {
            printf(" → ");
        }
    }
    printf("\n");
    
    free(visited);

    return original_tour;
}