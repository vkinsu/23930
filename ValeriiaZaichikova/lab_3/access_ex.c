#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void checkArgs (int argc) {
    if (argc != 2) {
        perror ("Invalid amount of arguments. Insert the filename.\n");
        exit(1);
    }
}

void getUIDs () {
    printf("real id: %d\neffective id: %d\n\n", getuid(), geteuid());
}

void changeEUID () {
    if (setuid(getuid())) {
        perror ("unable to change user id.\n\n");
        return;
    } else
        printf ("changed user id to: %d\n\n", geteuid());
}

void fileAccessCheck (char* filename) {
    FILE *file = fopen(filename, "r");

    if (file) {
        fclose (file);
        printf ("file opened succesfully.\n\n");
    } else {
        perror ("failed to open the file:\n");
        return;
    }
}

int main (int argc, char **argv){
    checkArgs (argc);

    getUIDs ();
    fileAccessCheck (argv[1]);

    changeEUID ();

    getUIDs ();
    fileAccessCheck (argv[1]);

    return 0;
}