#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void print_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    printf("RUID: %d\n", real_uid);
    printf("EUID: %d\n", effective_uid);
}

int main (int argc, char * argv[]) {
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		exit(0);
	}

    print_ids();

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File opened successfully.\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1) {
        perror("Error setting UID");
        exit(EXIT_FAILURE);
    }

    print_ids();
    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file after setuid");
    } else {
        printf("File opened successfully after setuid.\n");
        fclose(file);
    }

    exit(0);
}