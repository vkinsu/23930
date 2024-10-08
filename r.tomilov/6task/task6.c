#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LINES 500
#define BUFFER_SIZE 257

int file;
long lines_position[MAX_LINES];
int line_length[MAX_LINES];
int line_number = 1;

void handle_alarm(int sig) 
{
    char c;
    lseek(file, 0, SEEK_SET);
    printf("\n");
    while (read(file, &c, 1) > 0) 
    {
        write(1, &c, 1);
    }
    close(file);
    exit(0);
}


int main(int argc, char *argv[]) 
{
    char c, buffer[BUFFER_SIZE];

    int length = 0, exp_line_number;

    char lol[10];

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
            printf("line pos: %ld ", lines_position[line_number]);
            length++;
            line_length[line_number++] = length - 1;
            lines_position[line_number] = lseek(file, 0L, 1);
            length = 0;
            printf("lenghth: %i \n", line_length[line_number - 1]);
        }
        else
        {
            length++;
        }   
    }
    while(1) 
    {
        signal(SIGALRM, handle_alarm);
        alarm(5);

        printf("\n Enter line number: ");

        if (scanf("%s", lol) == 1) alarm(0);

        int valid_input = 1;
        for (int i = 0; i < strlen(lol); i++) 
        {
            if (!isdigit(lol[i])) 
            {
                valid_input = 0;
                break;
            }
        }

        if (valid_input)
        {
            exp_line_number = atoi(lol);

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
        else
        {
            printf("Invalid input: please enter a valid number\n");
        }
    }
}