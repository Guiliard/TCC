#include "reader.h"

char* read_file(char *filename) {
    FILE *arq = fopen(filename, "r"); 
    if (arq == NULL) {
        printf("Error: opening file\n");
        exit(1);
    }

    char *content = NULL;  
    size_t length = 0;     
    int ch;

    while ((ch = fgetc(arq)) != EOF) {
        content = (char*) realloc(content, length + 2);  
        if (content == NULL) {
            printf("Error: memory allocation failed to read content\n");
            exit(1);
        }
        content[length] = ch;
        length++;
        content[length] = '\0';
    }

    fclose(arq);
    return content;  
}

char* get_line_of_file(char *file, int line_number) {

    char* file_copy = strdup(file); 
    char *line = strtok(file_copy, "\n"); 

    for (int i = 0; i < line_number; i++) {
        if (line != NULL) {
            line = strtok(NULL, "\n");  
        } else {
            free(file_copy);
            return NULL;  
        }
    }

    char* line_copy = strdup(line);
    free(file_copy);
    return line_copy;  
}

unsigned short int count_lines(char *file) {
    unsigned short int count = 0;

    char* file_copy = strdup(file);

    char *line = strtok(file_copy, "\n"); 

    while (line != NULL) {
        count++; 
        line = strtok(NULL, "\n"); 
    }

    free(file_copy);

    return count; 
}