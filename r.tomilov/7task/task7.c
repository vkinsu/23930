#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define MAX_LINES 500

int file;
long lines_position[MAX_LINES];
int line_length[MAX_LINES];
int line_number = 1;

void handle_alarm(int sig) 
{
    char *mapped;
    struct stat sb;

    fstat(file, &sb);
    mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (mapped == MAP_FAILED) 
    {
        perror("mmap failed");
        exit(1);
    }
    printf("\n");
    write(1, mapped, sb.st_size);
    munmap(mapped, sb.st_size);
    close(file);
    exit(0);
}


int main(int argc, char *argv[]) 
{
    char *mapped;
    struct stat sb;

    int length = 0, exp_line_number;

    if((file =  open(argv[1], O_RDONLY)) == -1) 
    {
        printf("Failed to open file.");
        exit(1);
    }

    fstat(file, &sb);
    mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (mapped == MAP_FAILED) 
    {
        perror("mmap failed");
        exit(1);
    }

    lines_position[1] = 0L;

    for (int i = 0; i < sb.st_size; i++) 
    {
        if (mapped[i] == '\n') 
        {
            line_length[line_number] = length + 1;
            lines_position[line_number] = i + 1; 
            line_number++; 
            length = 0; 
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

        printf("Enter line number: ");

        if (scanf("%d", &exp_line_number) == 1) alarm(0);

        if(exp_line_number == 0)
        {
            exit(0);
        }
        
        if(exp_line_number >= 1 && exp_line_number < line_number)
        {
            write(1, mapped + lines_position[exp_line_number - 1], line_length[exp_line_number]);
        }   
        else
        {
             printf("Invalid line number\n");
        }
    }
    munmap(mapped, sb.st_size);
    close(file);
}