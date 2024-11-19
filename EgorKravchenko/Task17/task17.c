#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main() {
    struct termios old_tio;
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    char line[41] = {0};
    int pos = 0;
    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);
        if (c == 4 && pos == 0) {break;}
        if (c == 127) {
            if (pos > 0) {
                (pos)--;
                line[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            } else {
                putchar(7);
                fflush(stdout);
            }
        }
        if (c == 21) {
            while (pos > 0) {
                (pos)--;
                line[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }
        if (c == 23) {
            while (pos > 0 && line[pos - 1] == ' ') {
                (pos)--;
                line[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
            while (pos > 0 && line[pos - 1] != ' ') {
                (pos)--;
                line[pos] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }
        if (c >= 32 && c <= 126) {
            if (pos < 40) {
                line[pos++] = c;
                putchar(c);
                fflush(stdout);
                if (pos == 40) {
                    putchar('\n');
                    fflush(stdout);
                    pos = 0;
                }
            } else {
                putchar(7);
                fflush(stdout);
            }
        } else {
            putchar(7);
            fflush(stdout);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    return 0;
}


