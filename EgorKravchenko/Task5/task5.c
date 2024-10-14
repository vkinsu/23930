#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    long displ[500];
    int line_len[500];
    int file, i = 1, j = 0, line_no;
    char c, buf[257], input[257];
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ((file = open(argv[1], O_RDONLY)) == -1) {
        perror("Fail to open file");
        exit(EXIT_FAILURE);
    }
    displ[0] = 0L;
    line_len[0] = 0;
    while (read(file, &c, 1)) {
        j++;
        if (c == '\n') {
            displ[i] = lseek(file, 0L, SEEK_CUR);
            line_len[i - 1] = j;
            i++;
            j = 0;
        }
    }
    if (j > 0) {
        displ[i] = lseek(file, 0L, SEEK_CUR);
        line_len[i - 1] = j;
    }
    printf("Line Number\tOffset\t\tLength\n");
    printf("-----------------------------------------\n");
    for (int k = 0; k < i; k++) {
        printf("%d\t\t%ld\t\t%d\n", k + 1, displ[k], line_len[k]);
    }
    printf("-----------------------------------------\n");
    while (1) {
        printf("Enter line number(0-exit): ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '\n') {
            fprintf(stderr, "You just entered NOTHING. Try again.\n");
            continue;
        }
        if (sscanf(input, "%d", &line_no) != 1) {
            fprintf(stderr, "NUMBER please)\n");
            continue;
        }
        if (line_no == 0) {
            close(file);
            exit(0);
        }
        if (line_no >= i) {
            fprintf(stderr, "No such line.\n");
            continue;
        }
        if (line_no < 0) {
            fprintf(stderr, "Are you kidding me?\n");
            continue;
        }
        lseek(file, displ[line_no - 1], SEEK_SET);
        int len_to_read = line_len[line_no - 1];
        if (read(file, buf, len_to_read) == len_to_read) {
            buf[len_to_read] = '\0';
            write(1, buf, len_to_read);
        }
    }
    close(file);
    return 0;
}
