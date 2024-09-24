#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

void printUserID() {
    printf ("Real User ID = %d\n",getuid());
    printf ("Effective User ID = %d\n\n",geteuid());
}

void fileOpening(char *file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        perror("Failed to open the file");
        printf("\n");
    } else {
        fclose(f);
    }
}

int main() {
    printUserID();

    char file[1000] = "";
    printf("Write filename:\n");
    gets(file);

    fileOpening(file);

    setuid(geteuid());

    printUserID();
    fileOpening(file);
    return 0;
}
