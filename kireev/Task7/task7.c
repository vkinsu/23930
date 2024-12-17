#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

off_t *offset_table = NULL;
char *file_content = NULL;
int fd = 0;
size_t file_size = 0;

void on_alarm(int sig_num) {
    printf("\nOperation timed out. File content:\n");

    write(STDOUT_FILENO, file_content, file_size);

    free(offset_table);
    munmap(file_content, file_size);
    close(fd);

    exit(0);
}

void build_offset_table(char *file_content, off_t **offset_table, size_t file_size, int *line_count) {
    off_t current_offset = 0;
    int count = 0;
    *line_count = 0;

    *offset_table = malloc(sizeof(off_t) * 10);
    (*offset_table)[count++] = 0;

    for (size_t i = 0; i < file_size; i++) {
        current_offset++;

        if (file_content[i] == '\n') {
            if (count >= 10 * (*line_count + 1)) {
                *offset_table = realloc(*offset_table, sizeof(off_t) * (count + 10));
            }
            (*offset_table)[count++] = current_offset;
            (*line_count)++;
        }
    }
}

void print_offset_table(off_t *offset_table, int line_count) {
    printf("Offset Table:\n");
    for (int i = 0; i < line_count; i++) {
        printf("Line %d: Offset %lld\n", i + 1, (long long)offset_table[i]);
    }
}

void print_line(char *file_content, off_t *offset_table, int line_num) {
    off_t line_start = offset_table[line_num];
    off_t line_end = offset_table[line_num + 1];
    int length = line_end - line_start;

    write(STDOUT_FILENO, &file_content[line_start], length);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        exit(1);
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0) {
        perror("Error getting file size");
        close(fd);
        exit(1);
    }
    file_size = file_stat.st_size;

    file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        exit(1);
    }

    offset_table = NULL;
    int line_count = 0;
    build_offset_table(file_content, &offset_table, file_size, &line_count);
	
	if (line_count == 0) {
		printf("Offset Table is empty\n");
		free(offset_table);
		close(fd);
		exit(0);
	}
	print_offset_table(offset_table, line_count);

    int line_num;
    while (1) {
        signal(SIGALRM, on_alarm);
        printf("\nEnter line number (0 to exit), you have 5 seconds: ");
        fflush(stdout);
        alarm(5);
        scanf("%d", &line_num);
        alarm(0);

        if (line_num == 0) {
            break;
        }

        if (line_num < 1 || line_num > line_count) {
            printf("Invalid line number!\n");
        } else {
            print_line(file_content, offset_table, line_num - 1);
        }
    }

    free(offset_table);
    munmap(file_content, file_size);
    close(fd);

    exit(0);
}
