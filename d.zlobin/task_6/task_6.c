#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 2048

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

int fd;
LineInfo *line_table;
int num_lines;

void handle_alarm(int sig) {
    printf("\nTime's up! Printing the entire file:\n");
    lseek(fd, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    free(line_table);
    close(fd);
    exit(0);
}

int build_line_table(int fd, LineInfo **table, int *num_lines) {
    off_t offset = 0;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int line_count = 0;
    size_t current_length = 0;
    int table_size = 10;

    *table = malloc(table_size * sizeof(LineInfo));
    if (*table == NULL) {
        perror("Memory allocation error");
        return -1;
    }

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            current_length++;
            if (buffer[i] == '\n') {
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
    }

    *num_lines = line_count;
    return 0;
}

void print_line(int fd, LineInfo *table, int line_number) {
    if (line_number < 0) {
        printf("Invalid line number.\n");
        return;
    }

    if (lseek(fd, table[line_number].offset, SEEK_SET) == -1) {
        perror("Error seeking");
        return;
    }

    if (table[line_number].length == 0) {
        printf("Line %d:\n", line_number + 1);
        return;
    }

    char *line = malloc(table[line_number].length + 1);
    if (line == NULL) {
        perror("Memory allocation error");
        return;
    }

    if (read(fd, line, table[line_number].length) != table[line_number].length) {
        perror("Error reading line");
        free(line);
        return;
    }

    line[table[line_number].length] = '\0';
    printf("Line %d: %s", line_number + 1, line);

    free(line);
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

    if (build_line_table(fd, &line_table, &num_lines) != 0) {
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
            print_line(fd, line_table, line_number - 1);
        } else {
            printf("Invalid line number.\n");
        }
    }

    free(line_table);
    close(fd);

    return 0;
}
