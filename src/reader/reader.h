#ifndef READER_H
#define READER_H

#include "../utils/libs.h"

char* read_file(char *filename);
char* get_line_of_file(char *file, int line_number);
unsigned short int count_lines(char *file);

#endif