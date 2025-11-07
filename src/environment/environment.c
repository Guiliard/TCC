#include "environment.h"

void init_environment(char *path, city **cities, int ***distances, unsigned int *num_cities, float percent, unsigned int *prize_goal) {
    char *content[] = {"_N", "_P", "_w100", "_C"};
    char filenames[4][25];

    for (unsigned int i = 0; i < 4; i++) {
        snprintf(filenames[i], sizeof(filenames[i]), "%s%s.pctsp", path, content[i]);
    }

    char *file_N = read_file(filenames[0]);
    *num_cities = atoi(file_N);
    free(file_N);

    city *cities_array = allocate_vector(sizeof(city), *num_cities);
    int **distances_matrix = allocate_matrix(*num_cities, *num_cities);

    char *file_P = read_file(filenames[1]);
    for (unsigned int i = 0; i < *num_cities; i++) {
        char *line = get_line_of_file(file_P, i);
        if (!line) break;
        unsigned int idx, prize;
        sscanf(line, "%u %u", &idx, &prize);
        cities_array[i].index_city = i;
        cities_array[i].prize = prize;
        free(line);
    }
    free(file_P);

    char *file_W = read_file(filenames[2]);
    for (unsigned int i = 0; i < *num_cities; i++) {
        char *line = get_line_of_file(file_W, i);
        if (!line) break;
        unsigned int idx, penalty;
        sscanf(line, "%u %u", &idx, &penalty);
        cities_array[i].penalty = penalty;
        free(line);
    }
    free(file_W);

    char *file_C = read_file(filenames[3]);
    unsigned int total_lines_C = count_lines(file_C);
    for (unsigned int i = 0; i < total_lines_C; i++) {
        char *line = get_line_of_file(file_C, i);
        if (!line) break;
        unsigned int from, to;
        float dist;
        sscanf(line, "%u %u %f", &from, &to, &dist);
        distances_matrix[from][to] = (int)dist;
        free(line);
    }
    free(file_C);

    unsigned int total_prize = 0;
    for (unsigned int i = 0; i < *num_cities; i++) {
        total_prize += cities_array[i].prize;
    }
    *prize_goal = (unsigned int)(total_prize * percent);

    *cities = cities_array;
    *distances = distances_matrix;
}