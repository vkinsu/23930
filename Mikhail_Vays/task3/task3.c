#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main() {
    printf("Real User ID = %d\n", getuid());
    printf("Effective ID = %d\n\n", geteuid());
    FILE* file = fopen("example", "r");
    if (!file) {
        perror("Error opening file");
    } 
    else {
        fclose(file);
    }

    setuid(getuid());

    printf("Real User ID = %d\n", getuid());
    printf("Effective ID = %d\n\n", geteuid());
    file = fopen("example", "r");
    if (!file) {
        perror("Error opening file");
    }
    else {
        fclose(file);
    }

    return 0;
}