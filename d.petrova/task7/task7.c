#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

int fd;
char *file_in_memory;
struct stat file_stat;

void timeout_handler(int sig) {
    printf("\nTime is up!\n\n");
    write(1, file_in_memory, file_stat.st_size);
    munmap(file_in_memory, file_stat.st_size);
    exit(0);
}


int main(int argc, char *argv[]) {
    long lines_pos[500];
    int i = 1, j = 0, line_number, line_ln[500];
    char c, buf[300];
    if(( fd =  open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open file.");
        exit(1);
    }
    if(fstat(fd, &file_stat) == -1) {
        perror("Failed to get file stats");
        exit(1);
    }
    file_in_memory = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(file_in_memory == MAP_FAILED) {
        perror("Failed to map file to memory");
        exit(1);
    }
    lines_pos[1] = 0L;
    for (int offset = 0; offset < file_stat.st_size; offset++) {
        if (file_in_memory[offset] == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = offset + 1;

            printf("%ld  ", lines_pos[i-1]);
            fwrite(&file_in_memory[lines_pos[i-1]], 1, line_ln[i-1]-1, stdout);
            printf("  %d\n", line_ln[i-1]-1);

            j = 0;
        } else {
            j++;
        }
    }
    signal(SIGALRM, timeout_handler);
    while(1) {
        printf("Enter line number. You have 5 seconds: ");
        alarm(5);
        if (scanf("%d", &line_number) == 1) {
            alarm(0);
            if(line_number == 0) {
                munmap(file_in_memory, file_stat.st_size);
                exit(0);
            }
            if (line_number >= i)
                printf("Invalid line number\n");
            else
                write(1, &file_in_memory[lines_pos[line_number]], line_ln[line_number]);
        }
        else {
            alarm(0);
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
        }
    }
}