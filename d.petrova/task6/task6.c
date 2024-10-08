#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

int fd;

void timeout_handler(int sig) {
    char c;
    printf("\nTime is up!\n\n");
    lseek(fd, 0L, SEEK_SET); 
    while (read(fd, &c, 1)) {
        write(1, &c, 1);
    }
    exit(0); 
}


int main(int argc, char *argv[]) {
    long lines_pos[500];
    int i = 1, j = 0, line_number, line_ln[500];
    char c, buf[300];
    if(( fd =  open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open file.");
        exit(1);
    }
    lines_pos[1] = 0L;
    while(read(fd, &c, 1)) {
        if(c == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = lseek(fd, 0L, 1);
            printf("String %d begins at %ld\n", i-1, lines_pos[i-1]);
            j = 0;
        }
        else
            j++;
    }
    signal(SIGALRM, timeout_handler);
    while(1) {
        printf("Enter line number. You have 5 seconds: ");
        alarm(5);
        if (scanf("%d", &line_number) == 1) {
            alarm(0);
            if(line_number == 0)
                exit(0);
            lseek(fd, lines_pos[line_number], 0);
            if(read(fd, buf, line_ln[line_number]))
                write(1, buf, line_ln[line_number]);
            else
                printf("Invalid line number\n");
        }
    }
}