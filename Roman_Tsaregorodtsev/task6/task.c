#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

typedef struct {
    long offset;
    long length;
} LineInfo;

void build_line_table(LineInfo **table, int *line_count) {
    int input = open("example.txt", O_RDONLY);
    if (input == -1) {
        perror("error");
        exit(1);
    }
    long offset = 0;
    long length = 0;
    char buffer[1];
    int line_index = 0;
    LineInfo *temp_table = NULL;
    while (read(input, buffer, 1) == 1) {
        if (buffer[0] == '\n') {
            temp_table = realloc(temp_table, (line_index + 1) * sizeof(LineInfo));
            temp_table[line_index].offset = offset;
            temp_table[line_index].length = length;
            line_index++;
            offset += length + 1;
            length = 0;
        } else {
            length++;
        }
    }
    if (length > 0) {
        temp_table = realloc(temp_table, (line_index + 1) * sizeof(LineInfo));
        temp_table[line_index].offset = offset;
        temp_table[line_index].length = length;
        line_index++;
    }
    close(input);
    *table = temp_table;
    *line_count = line_index;
}

void print_line(const LineInfo *table, int line_number, int *line_count) {
    int input = open("example.txt", O_RDONLY);
    if (input == -1) {
        perror("error");
        exit(1);
    }
    if (line_number < 0 || line_number >= *line_count) {
        printf("Invalid line number\n");
        close(input);
        return;
    }
    lseek(input, table[line_number].offset, SEEK_SET);
    char *buffer = (char *)malloc(table[line_number].length + 1);
    read(input, buffer, table[line_number].length);
    buffer[table[line_number].length] = '\0';
    printf("%s\n", buffer);
    free(buffer);
    close(input);
}

void print_file_content() {
    int input = open("example.txt", O_RDONLY);
    if (input == -1) {
        perror("error");
        exit(1);
    }

    char buffer[1024];
    int result;
    while ((result = read(input, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, result);
    }
    close(input);
}

void MyAlarm()
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
        print_line(table, num - 1, &line_count);
    }
    free(table);
    return 0;
}
