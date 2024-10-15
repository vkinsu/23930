#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>

char *file_map;
size_t filesize;

typedef struct {
    long offset;
    long length;
} LineInfo;

void build_line_table(LineInfo **table, int *line_count) {
    long offset = 0;
    long length = 0;
    int line_index = 0;
    size_t i = 0;
    LineInfo *temp_table = NULL;
    while (i != filesize-1) {
        if (file_map[i] == '\n') {
            temp_table = (LineInfo *)realloc(temp_table, (line_index + 1) * sizeof(LineInfo));
            temp_table[line_index].offset = offset;
            temp_table[line_index].length = length;
            line_index++;
            offset += length + 1;
            length = 0;
        } else {
            length++;
        }
        i++;
    }
    if (length > 0) {
        temp_table = (LineInfo *)realloc(temp_table, (line_index + 1) * sizeof(LineInfo));
        temp_table[line_index].offset = offset;
        temp_table[line_index].length = length;
        line_index++;
    }
    *table = temp_table;
    *line_count = line_index;
}

void print_str(LineInfo *table, int line_number, int *line_count){
    if (line_number < 0 || line_number >= *line_count) {
        printf("Invalid line number\n");
        return;
    }
    if (line_number < *line_count-1)
        {for(int i=table[line_number].offset; i<table[line_number+1].offset; i++){
            printf("%c", file_map[i]);
        }}
    else
        {for(int i=table[line_number].offset; i<=table[line_number].offset + table[line_number].length; i++){
            printf("%c", file_map[i]);
        }
        printf("\n");
        }
}


void print_file_content() {
    size_t i = 0;
    LineInfo *temp_table = NULL;
    while (i != filesize) {
        printf("%c", file_map[i]);
        i++;
    }
}

void MyAlarm ()
{
    printf("\nTime is up!\n");
    print_file_content();
    printf("\n");
    _Exit (0);
}

int string_to_long(char *str) {
    if(str[0] == '\n')
    {
        fprintf(stderr, "Invalid input\n");
        return 0;
    }
    for(int i = 0;i<strlen(str)-1;i++)
    {
        if(str[i] < '0' || str[i] > '9')
        {
            fprintf(stderr, "Invalid input\n");
            return 0;
        }
    }
    long val;
    val = atol(str);
    return (int)val;
}

int main() {
    LineInfo *table = NULL;
    int line_count = 0;
    int input = open("example.txt", O_RDONLY);
    struct stat f_stat;
    if (fstat(input, &f_stat) == -1) {
        perror("error with getting file size");
        close(input);
        return 1;
    }
    filesize = f_stat.st_size;
    file_map = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, input, 0);
    if (file_map == MAP_FAILED) {
        perror("error with mmap");
        close(input);
        return 1;
    }
    build_line_table(&table, &line_count);
    // printf("Line table:\n");
    // for (int i = 0; i < line_count; i++) {
    //     printf("Line %d: Offset = %ld, Length = %ld\n", i, table[i].offset, table[i].length);
    // }
    char line_number[256];
    int num;
    while (1) {
        printf("You have 5 seconds to write a number\n");
        signal (SIGALRM, MyAlarm);
        alarm(5);
        printf("Enter line number: ");
        fgets(line_number, sizeof(line_number), stdin);
        alarm(0);
        num = string_to_long(line_number);
        if (num == 0) {
            break;
        }
        print_str(table, num-1, &line_count);
    }
    if (munmap(file_map, filesize) == -1)
        perror("error with munmap");
    free(table);
    close(input);
    return 0;
}
