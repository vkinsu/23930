#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

int fd;
LineInfo *line_table;
int num_lines;
char *file_data;
size_t file_size;

void handle_alarm(int sig) {
    printf("\nTime's up! Printing the entire file:\n");
    write(STDOUT_FILENO, file_data, file_size);
    free(line_table);
    munmap(file_data, file_size);
    close(fd);
    exit(0);
}

int build_line_table(LineInfo **table, int *num_lines) {
    off_t offset = 0;
    size_t current_length = 0;
    int line_count = 0;
    int table_size = 10;

    *table = malloc(table_size * sizeof(LineInfo));
    if (*table == NULL) {
        perror("Memory allocation error");
        return -1;
    }

    for (size_t i = 0; i < file_size; i++) {
        current_length++;
        if (file_data[i] == '\n') {
            if (line_count >= table_size) {
                table_size *= 2;
                *table = realloc(*table, table_size * sizeof(LineInfo));
                if (*table == NULL) {
                    perror("Memory allocation error");
                    return -1;
                }
            }
            (*table)[line_count].offset = offset;
            (*table)[line_count].length = (current_length == 1) ? 0 : current_length;
            offset += current_length;
            current_length = 0;
            line_count++;
        }
    }

    *num_lines = line_count;
    return 0;
}

void print_line(LineInfo *table, int line_number) {
    if (line_number < 0) {
        printf("Invalid line number.\n");
        return;
    }

    if (table[line_number].length == 0) {
        printf("Line %d:\n", line_number + 1);
        return;
    }

    printf("Line %d: ", line_number + 1);
    write(STDOUT_FILENO, &file_data[table[line_number].offset], table[line_number].length);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    signal(SIGALRM, handle_alarm);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return 1;
    }

    file_size = sb.st_size;

    file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    if (build_line_table(&line_table, &num_lines) != 0) {
        munmap(file_data, file_size);
        close(fd);
        return 1;
    }

    printf("Line table:\n");
    for (int i = 0; i < num_lines; i++) {
        printf("Line %d: offset = %lld, length = %zu\n", i + 1, (long long)line_table[i].offset, line_table[i].length);
    }

    int line_number;
    while (1) {
        printf("Enter line number (0 to exit, 5 seconds to respond): ");
        alarm(5);
        int result = scanf("%d", &line_number);
        alarm(0);

        if (result != 1) {
            printf("Invalid input. Please enter a valid line number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (line_number == 0) {
            break;
        } else if (line_number > 0 && line_number <= num_lines) {
            print_line(line_table, line_number - 1);
        } else {
            printf("Invalid line number.\n");
        }
    }

    free(line_table);
    munmap(file_data, file_size);
    close(fd);

    return 0;
}
