#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void print_ids() {
    printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
    printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
}

int main(int argc, char *argv[]) {

    char* file_name = argv[1];
    print_ids();
    FILE * fp = fopen(file_name, "r");
    if (fp != NULL) {
        printf("File opened successfully\n");
        fclose(fp);
    }
    else {
        printf("Failed to open file.\n");
    }
    setuid(getuid());
    FILE * fp2 = fopen(file_name, "r");
    if (fp2 != NULL) {
        printf("File opened successfully\n");
        fclose(fp2);
    }
    else {
        printf("Failed to open file.\n");
    }
}