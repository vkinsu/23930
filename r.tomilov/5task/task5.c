#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_LINES 500
#define BUFFER_SIZE 257

int main(int argc, char *argv[]) 
{
    long lines_position[MAX_LINES];
    int file;

    int line_number = 1, length = 0, exp_line_number, line_length[MAX_LINES];
    char c, buffer[BUFFER_SIZE];

    if((file =  open(argv[1], O_RDONLY)) == -1) 
    {
        printf("Failed to open file.");
        exit(1);
    }

    lines_position[1] = 0L;

    while(read(file, &c, 1)) 
    {
        if(c == '\n') 
        {
            length++;
            line_length[line_number++] = length;
            lines_position[line_number] = lseek(file, 0L, 1);
            length = 0;
        }
        else
        {
            length++;
        }   
    }
    while(1) 
    {
        printf("Enter line number: ");

        scanf("%d", &exp_line_number);

        if(exp_line_number == 0)
        {
            exit(0);
        }
            
        lseek(file, lines_position[exp_line_number], 0);

        if(read(file, buffer, line_length[exp_line_number]))
        {
            write(1, buffer, line_length[exp_line_number]);
        }   
        else
        {
             printf("Invalid line number\n");
        }
    }
}