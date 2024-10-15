#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 2048

typedef struct {
    off_t offset;
    size_t length;
} textTable;

int fName;
textTable *indTable;
int linesNum;

void handle_alarm(int sig) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("\nGame over. You lost. System will be removed.\n");
    lseek(fName, 0, SEEK_SET);

    while ((bytes_read = read(fName, buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    free(indTable);
    close(fName);
    exit(0);
}

int buildLineTable(int fd, textTable **table, int *linesNum) {
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
                
                curLineNum++;

                offset += lineLength;
                lineLength = 0;
            }
        }
    }

    *linesNum = curLineNum;
    return 0;
}

void printLine(int fd, textTable *table, int lineNum) {

    if (lseek(fd, table[lineNum].offset, SEEK_SET) == -1) {
        perror("Error seeking");
        return;
    }
    char *line = malloc(table[lineNum].length + 1);
    if (table[lineNum].length == 0) {
        printf("Line %d:\n", lineNum + 1);
        return;
    } else if (read(fd, line, table[lineNum].length) != table[lineNum].length) {
        free(line);
        return;
    }

    line[table[lineNum].length] = '\0';
    printf("Line %d: %s", lineNum + 1, line);

    free(line);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect format\n", argv[0]);
        return 1;
    }

    signal(SIGALRM, handle_alarm);

    fName = open(argv[1], O_RDONLY);
    if (fName == -1) {
        perror("This file doesn't exist");
        return 1;
    }

    if (buildLineTable(fName, &indTable, &linesNum) != 0) {
        perror("Error while building the table");
        close(fName);
        return 1;
    }

    printf("Line table:\n");
    for (int i = 0; i < linesNum; i++) {
        printf("Line %d: offset = %lld, length = %zu\n", i + 1, (long long)indTable[i].offset, indTable[i].length);
    }

    int lineNum;
    while (1)
    {
        printf("Enter line number (0 to exit, you have only 5 seconds!): ");
        alarm(5);
        int result = scanf("%d", &lineNum);
        

        if (result != 1) {
            printf("Invalid input. Please enter a valid line number.\n");
            while (getchar() != '\n');
            continue;
        }

        if (lineNum == 0) break;
        else if (lineNum > linesNum) printf("Invalid line number.\n");
        else {
            alarm(0);
            printLine(fName, indTable, lineNum - 1);
        }
    }

    close(fName);
    free(indTable);
    
    return 0;
}



