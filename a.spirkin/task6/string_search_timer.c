#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

char buffer[1024];
int lines = 0;
int length;
int f;
off_t line_start;
off_t line_end;
off_t *offset_table;

void alarm_handler(int sig) {
    printf("Time's out.\n");

    lseek(f, 0, SEEK_SET);

    line_start = offset_table[0];
    line_end = offset_table[lines];
    length = line_end - line_start;
    read(f, buffer, length);
    buffer[length] = '\0';
    printf("%s", buffer);

    free(offset_table);
    close(f);

    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("File name required.");
    }

    f = open(argv[1], O_RDWR);
    if (f == -1) {
        perror("Error on opening the file.");
        return 1;
    }

    offset_table = malloc(sizeof(off_t) * 10);
    off_t current_offset = 0;

    int bytes_read;
    int count = 0;

    offset_table[count++] = 0;

    while ((bytes_read = read(f, buffer, 1024)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            current_offset++;

            if (buffer[i] == '\n') {
                if (count >= 10 * (lines + 1)) {
                    offset_table = realloc(offset_table, sizeof(off_t) * (count + 10));
                }
                offset_table[count++] = current_offset;
                lines++;
            }
        }
    }

    if (lines == 0) {
        perror("No file lines detected.");
		free(offset_table);
		close(f);
		
        return 0;
	}

    int line_num;

    while (1) {
        signal(SIGALRM, alarm_handler);
        fflush(stdout);
        alarm(5);
        scanf("%d", &line_num);
        alarm(0);

        if (line_num == 0) {
            break;
        }

        if (line_num < 1 || line_num > lines) {
            printf("There's no string matching this line.\n");
        }
        else {
            line_num--;

            line_start = offset_table[line_num];
            line_end = offset_table[line_num + 1];
            length = line_end - line_start;

            lseek(f, line_start, SEEK_SET);

            read(f, buffer, length);
            buffer[length] = '\0';
            printf("%s", buffer);
        }
    }

    free(offset_table);
    close(f);

    return 0;
}
