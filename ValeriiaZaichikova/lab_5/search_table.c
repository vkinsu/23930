#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

typedef struct {
    off_t position;
    off_t size;
} Line;

typedef struct {
    Line *array;
    int cnt;
    int cap;
} Array;

void initArray(Array *a) {
    a->array = malloc(sizeof(Line));
    a->cnt = 0;
    a->cap = 1;
}

void insertArray(Array *a, Line element) {
    if (a->cnt == a->cap) {
        a->cap *= 2;
        a->array = realloc(a->array, a->cap * sizeof(Line));
    }

    a->array[a->cnt++] = element;
}

void freeArray(Array *a) {
    free(a->array);
    a->array = NULL;
    a->cnt = a->cap = 0;
}

void checkArgs (int argc) {
    if (argc != 2) { 
        printf ("\nInvalid number of arguments.\nPlease insert the filename.\n\n");
        exit (1);
    }
}

void checkFile (int file) {
    if (file == -1) { 
        printf ("\nUnable to open the file.\n\n");
        exit(1);
    }
}

void printTable (Array table) {
    printf ("\nNumber of lines in file: %d\n", table.cnt);
    for (int num = 1; num <= table.cnt; num++) {
        printf ("Line position: %d", num);
        printf ("\tLine size: %ld\n", table.array[num - 1].size);
    }
    printf ("\n");
}

void create_table (off_t input, Array table) {
    char c;
    off_t lineposition = 0;
    off_t linesize = 0;
    while (read(input, &c, 1) == 1) {
        if (c == '\n') {
            Line current = {lineposition, linesize};
            insertArray(&table, current);

            lineposition += linesize + 1;
            linesize = 0;
        } else {
            linesize++;
        }
    }

    if (linesize > 0) {
        Line current = {lineposition, linesize};
        insertArray(&table, current);
    }

    printTable (table);

    while (1) {
        int num;
        printf("Enter the line position: ");
        scanf("%d", &num);

        if (num == 0) { break; }
        if (table.cnt < num) {
            printf("The file contains only %d line(s).\n", table.cnt);
            continue;
        }

        Line line = table.array[num - 1];
        char *buf = calloc(line.size + 1, sizeof(char));

        lseek(input, line.position, SEEK_SET);
        read(input, buf, line.size * sizeof(char));

        printf("\t%s\n", buf);
        free(buf);
    }
}

int main(int argc, char *argv[]) {
    checkArgs (argc);
    char *path = argv[1];

    Array table;
    initArray(&table);

    int input = open(path, O_RDONLY);
    checkFile (input);
    lseek(input, 0L, SEEK_CUR);

    create_table (input, table);

    close(input);
    freeArray(&table);

    return 0;
}