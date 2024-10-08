#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

int fd;
char *file_memory;
size_t file_size;

void handle_timeout(int sig) {
    printf("\nTime is up! Printing the entire file:\n");
    write(1, file_memory, file_size);
    exit(0);
}

int main(int argc, char *argv[]) {
    long lines_pos[500];  
    int line_ln[500];
    int i = 1, j = 0, line_number;
    char c;
    struct stat st;
    char input_buf[16];

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file");
        exit(1);
    }

    if (fstat(fd, &st) == -1) {
        perror("Failed to get file size");
        close(fd);
        exit(1);
    }
    file_size = st.st_size;

    file_memory = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_memory == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        exit(1);
    }

    signal(SIGALRM, handle_timeout);

    lines_pos[1] = 0L; 

    for (size_t k = 0; k < file_size; k++) {
        c = file_memory[k];
        if (c == '\n') {
            line_ln[i] = j + 1;  
            lines_pos[++i] = k + 1;  
            j = 0;  
        } else {
            j++; 
        }
    }

    while (1) {
        printf("Enter line number (0 to exit): ");
        alarm(5);
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
                printf("Error reading input.\n");
                continue;
            }

            int valid_input = 1; 
            for (int k = 0; k < strlen(input_buf) - 1; k++) { 
                if (!isdigit(input_buf[k])) {
                    valid_input = 0; 
                    break;
                }
            }

            if (!valid_input) {
                printf("Invalid input. Please enter a number.\n");
                continue;
            }

            line_number = atoi(input_buf); 
        alarm(0);

        if (line_number == 0) {
            break;  
        }

        if (line_number < 1 || line_number >= i) {
            printf("Invalid line number\n");
            continue;
        }

        printf("%.*s", line_ln[line_number], file_memory + lines_pos[line_number]);
    }

    munmap(file_memory, file_size);
    close(fd);

    return 0;
}
