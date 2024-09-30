#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void build_offset_table(int fd, off_t **offset_table, int *line_count) {
    char buffer[BUFFER_SIZE];
    off_t current_offset = 0;
    int bytes_read;
    int count = 0;
    *line_count = 0;

    *offset_table = malloc(sizeof(off_t) * 10);
    (*offset_table)[count++] = 0;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            current_offset++;

            if (buffer[i] == '\n') {
                if (count >= 10 * (*line_count + 1)) {
                    *offset_table = realloc(*offset_table, sizeof(off_t) * (count + 10));
                }
                (*offset_table)[count++] = current_offset;
                (*line_count)++;
            }
        }
    }
}

void print_line(int fd, off_t *offset_table, int line_num) {
    char buffer[BUFFER_SIZE];
    off_t line_start = offset_table[line_num];
    off_t line_end = offset_table[line_num + 1];
    int length = line_end - line_start;

    lseek(fd, line_start, SEEK_SET);

    read(fd, buffer, length);
    buffer[length] = '\0';
    printf("%s", buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        exit(1);
    }

    off_t *offset_table = NULL;
    int line_count = 0;
    build_offset_table(fd, &offset_table, &line_count);

    int line_num;
    while (1) {
        printf("\nEnter line number (0 to exit): ");
        scanf("%d", &line_num);

        if (line_num == 0) {
            break;
        }

        if (line_num < 1 || line_num > line_count) {
            printf("Invalid line number!\n");
        } else {
            print_line(fd, offset_table, line_num - 1);
        }
    }

    free(offset_table);
    close(fd);

    exit(0);
}