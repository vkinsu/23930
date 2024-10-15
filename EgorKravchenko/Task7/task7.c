#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

int file;
char *mapped_file = NULL;
size_t file_size;

void alarmagedon() {
    printf("\nYou're too slow, Dude) File content:\n");
    write(1, mapped_file, file_size);
    munmap(mapped_file, file_size);
    close(file);
    exit(0);
}

int main(int argc, char *argv[]) {
    long displ[500];
    int line_len[500];
    int i = 1, j = 0, line_no;
    char c, input[257];
    struct stat file_stat;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ((file = open(argv[1], O_RDONLY)) == -1) {
        perror("Fail to open file");
        exit(EXIT_FAILURE);
    }
    if (fstat(file, &file_stat) == -1) {
        perror("Fail to get file size");
        close(file);
        exit(EXIT_FAILURE);
    }
    file_size = file_stat.st_size;
    mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (mapped_file == MAP_FAILED) {
        perror("Fail to map file");
        close(file);
        exit(EXIT_FAILURE);
    }
    displ[0] = 0L;
    line_len[0] = 0;
    for (size_t pos = 0; pos < file_size; pos++) {
        j++;
        if (mapped_file[pos] == '\n') {
            displ[i] = pos + 1;
            line_len[i - 1] = j;
            i++;
            j = 0;
        }
    }
    if (j > 0) {
        displ[i] = file_size;
        line_len[i - 1] = j;
    }
    printf("Line Number\tOffset\t\tLength\n");
    printf("-----------------------------------------\n");
    for (int k = 0; k < i; k++) {
        printf("%d\t\t%ld\t\t%d\n", k + 1, displ[k], line_len[k]);
    }
    printf("-----------------------------------------\n");
    signal(SIGALRM, alarmagedon);
    printf("You have 5 seconds to do something. Hurry Up!\n");
    alarm(5);
    while (1) {
        printf("Enter line number(0-exit): ");
        fgets(input, sizeof(input), stdin);
        alarm(0);
        if (input[0] == '\n') {
            fprintf(stderr, "You just entered NOTHING. Try again.\n");
            continue;
        }
        if (sscanf(input, "%d", &line_no) != 1) {
            fprintf(stderr, "NUMBER please)\n");
            continue;
        }
        if (line_no == 0) {
            munmap(mapped_file, file_size);
            close(file);
            exit(0);
        }
        if (line_no >= i) {
            fprintf(stderr, "No such line.\n");
            continue;
        }
        if (line_no < 0) {
            fprintf(stderr, "Are you kidding me?\n");
            continue;
        }
        write(1, &mapped_file[displ[line_no - 1]], line_len[line_no - 1]);
    }
    munmap(mapped_file, file_size);
    close(file);
    return 0;
}
