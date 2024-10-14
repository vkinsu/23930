#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUFFER_SIZE 2048

typedef struct {
    off_t offset;
    size_t length;
} textTable;

size_t fSize;
textTable *indTable;

int fName;
int linesNum;
char *fData;


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

int buildLineTable(textTable **table, int *linesNum) {
    off_t offset = 0;
    size_t lineLength = 0;

    int curLineCount = 0;
    int curTableSize = 5;

    *table = malloc(curTableSize * sizeof(textTable));

    for (size_t i = 0; i < fSize; i++) {
        lineLength++;
        if (fData[i] == '\n') {

            if (curLineCount >= curTableSize) {
                curTableSize *= 2;
                *table = realloc(*table, curTableSize * sizeof(textTable));
            }

            (*table)[curLineCount].offset = offset;
            (*table)[curLineCount].length = (lineLength == 1) ? 0 : lineLength;

            curLineCount++;
            
            offset += lineLength;
            lineLength = 0;
        }
    }

    *linesNum = curLineCount;
    return 0;
}

void printLine(textTable *table, int lineNum) {
    if (lineNum < 0) {
        printf("Invalid line number.\n");
        return;
    }

    if (table[lineNum].length == 0) {
        printf("Line %d:\n", lineNum + 1);
        return;
    }

    printf("Line %d: ", lineNum + 1);
    write(STDOUT_FILENO, &fData[table[lineNum].offset], table[lineNum].length);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Incorrect format\n", argv[0]);
        return 1;
    } else {
        fName = open(argv[1], O_RDONLY);
        if (fName == -1) {
            perror("This file doesn't exist");
            return 1;
        }
    }
    
    struct stat sb;
    if (fstat(fName, &sb) == -1) {
        perror("Error getting file size");
        close(fName);
        return 1;
    }
    fSize = sb.st_size;

    signal(SIGALRM, handle_alarm);
    fSize = sb.st_size;
    fData = mmap(NULL, fSize, PROT_READ, MAP_PRIVATE, fName, 0);

    if (buildLineTable(&indTable, &linesNum) != 0) {
        perror("Error while building the table");
        munmap(fData, fSize);
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
            printLine(indTable, lineNum - 1);
        }
    }

    free(indTable);
    munmap(fData, fSize);
    close(fName);

    return 0;
}

