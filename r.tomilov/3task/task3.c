#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 255

int main() {
    char filename[MAX_FILENAME_LENGTH];

    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    printf("Real UID: %d\n", real_uid);
    printf("Effective UID: %d\n", effective_uid);

    printf("Enter the filename: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Error reading filename");
        return EXIT_FAILURE;
    }

    filename[strcspn(filename, "\n")] = '\0';

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fclose(file);

    if (setuid(getuid()) == -1) {
        perror("setuid failed");
        return EXIT_FAILURE;
    }

    real_uid = getuid();
    effective_uid = geteuid();
    
    printf("Real UID after file operations: %d\n", real_uid);
    printf("Effective UID after file operations: %d\n", effective_uid);

    FILE *file2 = fopen(filename, "w");
    if (file2 == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fclose(file2);

    return EXIT_SUCCESS;
}
