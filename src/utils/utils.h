#ifndef UTILS_H
#define UTILS_H

#include "libs.h"

#define max(x,y) ((x)<(y) ? (y) : (x))
#define min(x,y) ((x)>(y) ? (y) : (x))

#define INT_MAX __INT_MAX__
#define FLT_MAX	__FLT_MAX__

typedef struct city {
	unsigned int index_city;
    unsigned int prize;
    unsigned int penalty;
} city;

void *allocate_vector(size_t element_size, size_t count);
int **allocate_matrix(unsigned int num_rows, unsigned int num_cols);

void print_cities(city *cities, unsigned int num_cities);
void print_matrix(int **matrix, unsigned int num_rows, unsigned int num_cols);
void print_tour(int *tour, unsigned int tour_size);

#endif