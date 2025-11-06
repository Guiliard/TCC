#include "local_search.h"

int* copy_tour(int *tour, unsigned int tour_size) {
    int *new_tour = (int *)allocate_vector(sizeof(int), tour_size);
    for (unsigned int i = 0; i < tour_size; i++) {
        new_tour[i] = tour[i];
    }
    return new_tour;
}

bool is_city_in_tour(int city, int *tour, unsigned int tour_size) {
    for (unsigned int i = 0; i < tour_size; i++) {
        if (tour[i] == city) {
            return true;
        }
    }
    return false;
}

int* local_search_insertion(int *current_tour, unsigned int current_tour_size, int **distances, city *cities, unsigned int num_cities, unsigned int k, float *best_fo) {
    
    printf("\n=== INICIANDO BUSCA LOCAL POR INSERÇÃO ===\n");
    printf("Tour atual: ");
    print_tour(current_tour, current_tour_size);
    printf("FO atual: %.2f\n", *best_fo);
    printf("Analisando %u cidades para inserção\n", k);
    
    float current_fo = *best_fo;
    
    unsigned int non_visited_count = 0;
    
    for (unsigned int i = 1; i < num_cities; i++) {
        if (!is_city_in_tour(i, current_tour, current_tour_size)) {
            non_visited_count++;
        }
    }
    
    printf("Cidades não visitadas disponíveis: %u\n", non_visited_count);
    
    if (non_visited_count == 0) {
        printf("Nenhuma cidade não visitada disponível. Retornando tour atual.\n");
        return copy_tour(current_tour, current_tour_size);
    }
    
    int *non_visited = (int *)allocate_vector(sizeof(int), non_visited_count);
    unsigned int count = 0;
    
    for (unsigned int i = 1; i < num_cities; i++) {
        if (!is_city_in_tour(i, current_tour, current_tour_size)) {
            non_visited[count++] = i;
        }
    }
    
    if (k > non_visited_count) {
        k = non_visited_count;
        printf("Ajustando k para %u (número de cidades não visitadas disponíveis)\n", k);
    }
    
    int *candidate_cities = (int *)allocate_vector(sizeof(int), k);
    bool *selected = (bool *)allocate_vector(sizeof(bool), non_visited_count);
    
    printf("Cidades candidatas selecionadas: ");
    for (unsigned int i = 0; i < k; i++) {
        unsigned int random_idx;
        do {
            random_idx = rand() % non_visited_count;
        } while (selected[random_idx]);
        
        candidate_cities[i] = non_visited[random_idx];
        selected[random_idx] = true;
        printf("%d ", candidate_cities[i]);
    }
    printf("\n");
    
    int *best_neighbor = NULL;
    float best_neighbor_fo = current_fo;
    
    for (unsigned int c = 0; c < k; c++) {
        int candidate_city = candidate_cities[c];
        printf("\n--- Testando cidade %d ---\n", candidate_city);
        
        for (unsigned int pos = 1; pos < current_tour_size; pos++) {
            unsigned int new_tour_size = current_tour_size + 1;
            int *new_tour = (int *)allocate_vector(sizeof(int), new_tour_size);
            
            for (unsigned int i = 0; i < pos; i++) {
                new_tour[i] = current_tour[i];
            }
            new_tour[pos] = candidate_city;
            for (unsigned int i = pos; i < current_tour_size; i++) {
                new_tour[i + 1] = current_tour[i];
            }
            
            print_tour(new_tour, new_tour_size);

            float new_fo = calculate_objective_function(new_tour, new_tour_size, distances, cities, num_cities);
            
            printf("  Posição %u: FO = %.2f", pos, new_fo);
            
            if (new_fo < best_neighbor_fo) {
                best_neighbor_fo = new_fo;
                if (best_neighbor != NULL) {
                    free(best_neighbor);
                }
                best_neighbor = copy_tour(new_tour, new_tour_size);
                printf(" ← NOVO MELHOR!");
            }
            printf("\n");
            
            free(new_tour);
        }
    }
    
    if (best_neighbor != NULL) {
        printf("\n✅ MELHOR VIZINHO ENCONTRADO!\n");
        printf("FO anterior: %.2f\n", current_fo);
        printf("FO nova: %.2f\n", best_neighbor_fo);
        printf("Melhoria: %.2f\n", current_fo - best_neighbor_fo);
        *best_fo = best_neighbor_fo;
    } else {
        printf("\n❌ NENHUM VIZINHO MELHOR ENCONTRADO\n");
        best_neighbor = copy_tour(current_tour, current_tour_size);
    }
    
    free(non_visited);
    free(candidate_cities);
    free(selected);
    
    return best_neighbor;
}