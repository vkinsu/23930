#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    printf("Enter your lines:\n");
    char* buffer = calloc(4096, sizeof(char));
    int cnt = 0;
    char** lines = calloc(cnt + 1, sizeof(char*));

    while (fgets(buffer, 4096, stdin)) {
        if (*buffer == '.') {
            for (int i = 0; i < cnt; ++i) {
                printf("%s", lines[i]);
                free(lines[i]);
            }
            free(lines);
            break;
        }

        void* new_ptr = realloc(lines, (cnt + 1) * sizeof(char*));
        if (new_ptr == NULL) exit(EXIT_FAILURE);
        else lines = new_ptr;
        

        char* line = malloc((strlen(buffer) + 1) * sizeof(char));
        if (line == NULL) exit(EXIT_FAILURE);

        strcpy(line, buffer);
        lines[cnt] = line;
        ++cnt;
    }

    free(buffer);
    return 0;
}
