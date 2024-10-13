#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 2048

typedef struct {
    off_t offset;
    size_t length;
} textTable;

int build_line_table(int fd, textTable **table, int *linesNum) {
    char buffer[BUFFER_SIZE];
    off_t offset = 0;
    ssize_t symRead;
    size_t lineLength = 0;
    
    int curLineNum = 0;
    int curTableSize = 5;

    *table = malloc(curTableSize * sizeof(textTable));

    while ((symRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < symRead; i++) {
            lineLength++;
            if (buffer[i] == '\n') {

                if (curLineNum >= curTableSize) {
                    curTableSize *= 2;
                    *table = realloc(*table, curTableSize * sizeof(textTable));
                }

                (*table)[curLineNum].offset = offset;
                (*table)[curLineNum].length = (lineLength == 1) ? 0 : lineLength;

                offset += lineLength;
                lineLength = 0;
                curLineNum++;
            }
        }
    }

    *linesNum = curLineNum;
    return 0;
}

void print_line(int fd, textTable *table, int line_number) {

    if (lseek(fd, table[line_number].offset, SEEK_SET) == -1) {
        perror("Error seeking");
        return;
    }

    if (table[line_number].length == 0) {
        printf("Line %d:\n", line_number + 1);
        return;
    }

    char *line = malloc(table[line_number].length + 1);

    if (read(fd, line, table[line_number].length) != table[line_number].length) {
        free(line);
        return;
    }

    line[table[line_number].length] = '\0';
    printf("Line %d: %s", line_number + 1, line);

    free(line);
}

int main(int argc, char *argv[]) {

    textTable *line_table;
    int linesNum;

    if (argc != 2) {
        printf("Wrong number of args\n");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("Invalid file name");
        return 1;
    }

    if (build_line_table(fd, &line_table, &linesNum) != 0) {
        close(fd);
        return 1;
    }

    printf("Line table:\n");
    for (int i = 0; i < linesNum; i++) {
        printf("Line %d: offset = %lld, length = %zu\n", i + 1, (long long)line_table[i].offset, line_table[i].length);
    }

    int line_number;
    while (1) {
        printf("Enter line number (0 to exit): ");
        int result = scanf("%d", &line_number);

        if (result != 1) {
            printf("Invalid input. Please enter a valid line number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (line_number == 0) {
            break;
        } else if (line_number > linesNum) {
            printf("Invalid line number.\n");
        } else if(line_number > 0 && line_number <= linesNum){
            print_line(fd, line_table, line_number - 1);
        }
    }

    free(line_table);
    close(fd);

    return 0;
}
