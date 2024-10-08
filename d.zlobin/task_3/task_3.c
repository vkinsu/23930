#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

void print_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    printf("Real UID: %d, Efficient UID: %d\n", real_uid, effective_uid);
}

int main() {

    FILE *file;
    print_ids();

    file = fopen("file.txt", "r");
    if (file == NULL) {
        perror("File opening error!\n");
    } else {
        printf("File has been opened succesfully!\n");
        fclose(file);
    }

    if (setuid(getuid()) != 0) {
        perror("Execution setuid error!\n");
        return EXIT_FAILURE;
    }

    printf("\nAfter setuid() method using:\n");
    print_ids();
    
    file = fopen("file.txt", "r");
    if (file == NULL) {
        perror("File opening error after setuid() method using!\n");
    } else {
        printf("File has been opened succesfully after setuid() method using!\n");
        fclose(file);
    }

    return EXIT_SUCCESS;
}
