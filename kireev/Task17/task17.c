#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 40
#define ERASE 127      // Символ для удаления (Backspace)
#define KILL 21        // CTRL-U для стирания всей строки
#define CTRL_W 23      // CTRL-W для удаления последнего слова
#define CTRL_D 4       // CTRL-D для выхода
#define CTRL_G 7       // CTRL-G для звукового сигнала

struct termios original;

void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

void configure_terminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    original = t;
    atexit(reset_terminal);

    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void print_ctrl_g() {
    putchar(CTRL_G);
    fflush(stdout);
}

int main() {
    configure_terminal();

    char line[MAX_LINE_LENGTH + 1] = {0};
    int length = 0;

    while (1) {
        int ch = getchar();

        if (ch == CTRL_D && length == 0) {
            break;
        } else if (ch == ERASE) {
            if (length > 0) {
                length--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (ch == KILL) {
            while (length > 0) {
                printf("\b \b");
                length--;
            }
            fflush(stdout);
        } else if (ch == CTRL_W) {
            while (length > 0 && line[length - 1] == ' ') {
                printf("\b \b");
                length--;
            }
            while (length > 0 && line[length - 1] != ' ') {
                printf("\b \b");
                length--;
            }
            fflush(stdout);
        } else if (isprint(ch)) {
            if (length < MAX_LINE_LENGTH) {
                line[length++] = ch;
                putchar(ch);
            } else {
                putchar('\n');
                line[0] = ch;
                length = 1;
                putchar(ch);
            }
            fflush(stdout);
        } else {
            print_ctrl_g();
        }
    }

    exit(0);
}
