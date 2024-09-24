#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main() {

    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    printf("Current UID: %d\n", getuid());
    printf("Needed UID: %d\n", geteuid());


    FILE *file = fopen("file.txt", "r");

    if (file == NULL) perror("File opening error!\n");
    else {
        printf("File opened succesfully!\n");
        fclose(file);
    }


    if (setuid(real_uid) == -1) {
        perror("Execution setuid error!\n");
        exit(EXIT_FAILURE);
    }

    printf("Real and efficient UID are now the same.\n");

    printf("Current UID: %d\n", getuid());
    printf("Needed UID: %d\n", geteuid());

    file = fopen("file.txt", "r");

    if (file == NULL) perror("File opening error!\n");
    else {
        printf("File opened succesfully!\n");
        fclose(file);
    }

    return 0;
}
