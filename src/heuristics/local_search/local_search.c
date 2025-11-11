#include "local_search.h"

bool is_city_in_tour(int city, int *tour, unsigned int tour_size) {
    for (unsigned int i = 0; i < tour_size; i++) {
        if (tour[i] == city) {
            return true;
        }
    }
    return false;
}

void random_insertion(city *all_cities, unsigned int num_all_cities, unsigned int tour_size, int **tour) {    
    int *unvisited_cities = NULL;
    unsigned int num_unvisited = 0;
    
    for (unsigned int i = 0; i < num_all_cities; i++) {
        if (!is_city_in_tour(all_cities[i].index_city, *tour, tour_size)) {
            num_unvisited++;
        }
    }
    
    unvisited_cities = (int *)allocate_vector(sizeof(int), num_unvisited);
   
    unsigned int index = 0;
    for (unsigned int i = 0; i < num_all_cities; i++) {
        if (!is_city_in_tour(all_cities[i].index_city, *tour, tour_size)) {
            unvisited_cities[index++] = all_cities[i].index_city;
        }
    }
    
    printf("Cidades não visitadas disponíveis: %u\n", num_unvisited);
    
    unsigned int random_city_index = rand() % num_unvisited;
    int city_to_insert = unvisited_cities[random_city_index];
    
    unsigned int min_position = 1;
    unsigned int max_position = tour_size - 1;
    unsigned int random_position = min_position + (rand() % (max_position - min_position + 1));
    
    printf("Inserindo cidade %d na posição %d\n", city_to_insert, random_position);
    printf("Tour antes: ");
    for (unsigned int i = 0; i < tour_size; i++) {
        printf("%d", (*tour)[i]);
        if (i < tour_size - 1) printf(" → ");
    }
    printf("\n");
    
    int *new_tour = (int *)allocate_vector(sizeof(int), tour_size + 1);
    
    for (unsigned int i = 0; i < random_position; i++) {
        new_tour[i] = (*tour)[i];
    }
    new_tour[random_position] = city_to_insert;
    for (unsigned int i = random_position; i < tour_size; i++) {
        new_tour[i + 1] = (*tour)[i];
    }
    
    free(*tour);
    *tour = new_tour;
    
    free(unvisited_cities);
}

void random_swap(city *all_cities, unsigned int num_all_cities, unsigned int tour_size, int **tour) {
    int *unvisited_cities = NULL;
    unsigned int num_unvisited = 0;
    
    for (unsigned int i = 0; i < num_all_cities; i++) {
        if (!is_city_in_tour(all_cities[i].index_city, *tour, tour_size)) {
            num_unvisited++;
        }
    }
    
    unvisited_cities = (int *)allocate_vector(sizeof(int), num_unvisited);
    
    unsigned int index = 0;
    for (unsigned int i = 0; i < num_all_cities; i++) {
        if (!is_city_in_tour(all_cities[i].index_city, *tour, tour_size)) {
            unvisited_cities[index++] = all_cities[i].index_city;
        }
    }
    
    printf("Cidades não visitadas disponíveis: %u\n", num_unvisited);
    
    unsigned int random_unvisited_index = rand() % num_unvisited;
    int city_to_insert = unvisited_cities[random_unvisited_index];
    
    unsigned int min_position = 1;
    unsigned int max_position = tour_size - 2;
    unsigned int random_position = min_position + (rand() % (max_position - min_position + 1));
    
    int city_to_remove = (*tour)[random_position];
    
    printf("Trocando cidade %d (posição %d) por cidade %d\n", city_to_remove, random_position, city_to_insert);
    printf("Tour antes: ");
    for (unsigned int i = 0; i < tour_size; i++) {
        printf("%d", (*tour)[i]);
        if (i < tour_size - 1) printf(" → ");
    }
    printf("\n");
    
    int *new_tour = (int *)allocate_vector(sizeof(int), tour_size);
    
    for (unsigned int i = 0; i < tour_size; i++) {
        if (i == random_position) {
            new_tour[i] = city_to_insert;
        } else {
            new_tour[i] = (*tour)[i];
        }
    }
    
    free(*tour);
    *tour = new_tour;
    
    free(unvisited_cities);
}

void random_drop(unsigned int tour_size, int **tour) {
    if (tour_size <= 3) {
        printf("Tour muito pequeno para remover cidades. Tamanho atual: %u\n", tour_size);
        return;
    }

    printf("Tour atual: ");
    for (unsigned int i = 0; i < tour_size; i++) {
        printf("%d", (*tour)[i]);
        if (i < tour_size - 1) printf(" → ");
    }
    printf("\n");

    unsigned int min_position = 1;
    unsigned int max_position = tour_size - 2;
    unsigned int random_position = min_position + (rand() % (max_position - min_position + 1));
    
    int city_to_remove = (*tour)[random_position];
    
    printf("Removendo cidade %d da posição %d\n", city_to_remove, random_position);

    int *new_tour = (int *)allocate_vector(sizeof(int), tour_size - 1);
    
    for (unsigned int i = 0; i < random_position; i++) {
        new_tour[i] = (*tour)[i];
    }
    
    for (unsigned int i = random_position + 1; i < tour_size; i++) {
        new_tour[i - 1] = (*tour)[i];
    }
    
    free(*tour);
    *tour = new_tour;
}