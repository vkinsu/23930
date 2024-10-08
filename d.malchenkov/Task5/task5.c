    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <ctype.h>

    #define MAX_LINE_LENGTH 256

    int main(int argc, char *argv[]) {
        long lines_pos[500];
        int line_ln[500];
        int fd;
        int i = 1, j = 0, line_number;
        char c, buf[MAX_LINE_LENGTH + 1];
        char input_buf[16];

        if (argc < 2) {
            printf("Usage: %s <filename>\n", argv[0]);
            exit(1);
        }

        if ((fd = open(argv[1], O_RDONLY)) == -1) {
            printf("Failed to open file.\n");
            exit(1);
        }

        lines_pos[1] = 0L; // Первая строка начинается с нулевого отступа

        while (read(fd, &c, 1)) {
            if (c == '\n') {
                line_ln[i] = j + 1;  
                lines_pos[++i] = lseek(fd, 0L, SEEK_CUR);  // Сохраняем смещение начала новой строки
                j = 0;  
            } else {
                j++; 
            }
        }

        while (1) {
            printf("Enter line number (0 to exit): ");
            if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
                printf("Error reading input.\n");
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

            if (line_number == 0) {
                close(fd);
                exit(0);
            }

            if (line_number < 1 || line_number >= i) {
                printf("Invalid line number\n");
                continue;
            }

            // Перемещение на начало нужной строки
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
