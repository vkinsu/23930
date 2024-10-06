#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void opening(char *arg) {
    printf("\tUID: %d\n", getuid());
    printf("\tEUID: %d\n", geteuid());

    FILE *f = fopen(arg, "r");
    if (f) {
        fclose(f);
    }
    else {
        perror("Error on reading the file.");
    }
}

int main(int argc, char *argv[])
{
    opening(argv[1]);

    if (setuid(getuid()) == -1) {
        perror("Error on handling UID update.\n");
    }
    else {
        printf("Changed UID to EUID value.\n");
    }

    opening(argv[1]);

    return 0;
}
