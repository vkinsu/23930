#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    long lines_pos[500];
    int fd;
    int i = 1, j = 0, line_number, line_ln[500];
    char c, buf[257];
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
            j = 0;
        }
        else
            j++;
    }
    while(1) {
        printf("Enter line number: ");
        scanf("%d", &line_number);
        if(line_number == 0)
            exit(0);
        lseek(fd, lines_pos[line_number], 0);
        if(read(fd, buf, line_ln[line_number]))
            write(1, buf, line_ln[line_number]);
        else
            printf("Invalid line number\n");
    }
}