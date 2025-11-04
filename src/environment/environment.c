#include "environment.h"

void init_environment(char *path, city **cities, int ***distances, unsigned int *num_cities, float percent, unsigned int *prize_goal) {
    char *content[] = {"_N", "_P", "_w100", "_C"};
    char filenames[4][25];

    for (unsigned int i = 0; i < 4; i++) {
        snprintf(filenames[i], sizeof(filenames[i]), "%s%s.pctsp", path, content[i]);
    }

    char *file_N = read_file(filenames[0]);
    unsigned int num_cities_value = atoi(file_N);
    free(file_N);
    *num_cities = num_cities_value;

    city *cities_array = allocate_vector(sizeof(city), num_cities_value);
    int **distances_matrix = allocate_matrix(num_cities_value, num_cities_value);

    char *file_P = read_file(filenames[1]);
    char *file_W = read_file(filenames[2]);
    char *line;

    line = strtok(file_P, "\n");
    for (unsigned int i = 0; i < num_cities_value && line != NULL; i++) {
        unsigned int idx, prize;
        sscanf(line, "%u %u", &idx, &prize);
        cities_array[i].index_city = i;
        cities_array[i].prize = prize;

        line = strtok(NULL, "\n");
    }

    line = strtok(file_W, "\n");
    for (unsigned int i = 0; i < num_cities_value && line != NULL; i++) {
        unsigned int idx, penalty;
        sscanf(line, "%u %u", &idx, &penalty);
        cities_array[i].penalty = penalty;

        line = strtok(NULL, "\n");
    }

    free(file_P);
    free(file_W);

    char *file_C = read_file(filenames[3]);
    unsigned int from, to;
    float dist;
    line = strtok(file_C, "\n");
    while (line) {
        sscanf(line, "%u %u %f", &from, &to, &dist);
        distances_matrix[from][to] = dist;
        line = strtok(NULL, "\n");
    }
    free(file_C);

    *cities = cities_array;
    *distances = distances_matrix;

    unsigned int total_prize = 0;
    for(unsigned int i = 0; i < num_cities_value; i++) {
        total_prize += cities_array[i].prize;
    }

    *prize_goal = (unsigned int)(total_prize * percent);
}