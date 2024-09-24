#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048

void printLines(char** lines, int amountOfLines) {
    for (int i = 0; i < amountOfLines; ++i) {
        printf("%s\n", lines[i]);
        free(lines[i]);
    }
    free(lines);
}

int main() {
    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    int linesCounter = 0;
    char** lines = calloc(linesCounter + 1, sizeof(char*)); // +1 for the last \0

    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        if (*buffer == '.') {
            printLines(lines, linesCounter);
            break;
        }

        void* new_ptr = realloc(lines, (linesCounter + 1) * sizeof(char*));
        if (new_ptr == NULL) {
            fprintf(stderr, "Error allocating memory.\n");
            exit(EXIT_FAILURE);
        } else {
            lines = new_ptr;
        }

        char* line = malloc((strlen(buffer) + 1) * sizeof(char));
        if (line == NULL) {
            fprintf(stderr, "Error allocating memory.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(line, buffer);
        lines[linesCounter] = line;
        ++linesCounter;
    }

    free(buffer);
    return 0;
}