#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

int fd;

void handle_timeout(int sig) {
    char buf[MAX_LINE_LENGTH + 1];
    printf("\nTime is up! Printing the entire file:\n");

    lseek(fd, 0, SEEK_SET); 

    int bytes_read;
    while ((bytes_read = read(fd, buf, MAX_LINE_LENGTH)) > 0) {
        write(1, buf, bytes_read); 
    }

    close(fd);
    exit(0);  
}

int main(int argc, char *argv[]) {
    long lines_pos[500];
    int line_ln[500];
    int i = 1, j = 0, line_number;
    char c, buf[MAX_LINE_LENGTH + 1];
    char input_buf[16];

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Открытие файла
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        printf("Failed to open file.\n");
        exit(1);
    }

    signal(SIGALRM, handle_timeout);

    lines_pos[1] = 0L; 

    while (read(fd, &c, 1)) {
        if (c == '\n') {
            line_ln[i] = j + 1; 
            lines_pos[++i] = lseek(fd, 0L, SEEK_CUR);  
            j = 0; 
        } else {
            j++;
        }
    }

    printf("\nLines positions and lengths:\n");
    printf("Line Number\tPosition\tLength\n");
    for (int k = 1; k < i; k++) {
        printf("%d\t\t%ld\t\t%d\n", k, lines_pos[k], line_ln[k]);
    }
    printf("\n");


    while (1) {
        printf("Enter line number (0 to exit): ");
        alarm(5);
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
                printf("Error reading input.\n");
                continue;
        }

        if (input_buf[0] == '\n') {
            printf("No input detected. Please enter a line number.\n");
            continue;
        }

        int valid_input = 1; 
        for (int k = 0; k < strlen(input_buf) - 1; k++) { 
            if (!isdigit(input_buf[k])) {
                valid_input = 0; 
                   break;
            }
        }

        if (!valid_input) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        line_number = atoi(input_buf); 
        alarm(0);

        if (line_number == 0) {
            close(fd);
            exit(0);
        }

        if (line_number < 1 || line_number >= i) {
            printf("Invalid line number\n");
            continue;
        }

        lseek(fd, lines_pos[line_number], SEEK_SET);

        int bytes_read = read(fd, buf, line_ln[line_number]);
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';  
            printf("%s", buf); 
        } else {
            printf("Error reading line\n");
        }
    }

    close(fd);

    return 0;
}
