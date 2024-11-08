#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define ERASE 127         // Символ ERASE (Backspace)
#define KILL 21           // Символ KILL (CTRL-U)
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7
#define MAX_LINE_LEN 40

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void clear_line(char *buffer, int *length) {
    while (*length > 0) {
        write(STDOUT_FILENO, "\b \b", 3);  // https://ru.wikipedia.org/wiki/%D0%A3%D0%BF%D1%80%D0%B0%D0%B2%D0%BB%D1%8F%D1%8E%D1%89%D0%B8%D0%B5_%D1%81%D0%B8%D0%BC%D0%B2%D0%BE%D0%BB%D1%8B
        (*length)--;
    }
    buffer[0] = '\0';
}

void delete_last_word(char *buffer, int *length) {
    while (*length > 0 && buffer[*length - 1] == ' ') {
        write(STDOUT_FILENO, "\b \b", 3);
        (*length)--;
    }
    while (*length > 0 && buffer[*length - 1] != ' ') {
        write(STDOUT_FILENO, "\b \b", 3);
        (*length)--;
    }
    buffer[*length] = '\0';
}

int main() {
    enable_raw_mode();

    char buffer[MAX_LINE_LEN + 1] = {0};
    int length = 0;

    while (1) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) break;

        if (c == CTRL_D && length == 0) {
            break;
        } else if (c == ERASE) {
            if (length > 0) {
                write(STDOUT_FILENO, "\b \b", 3);
                length--;
                buffer[length] = '\0';
            }
        } else if (c == KILL) {
            clear_line(buffer, &length);
        } else if (c == CTRL_W) {
            delete_last_word(buffer, &length);
        } else if (c == '\n') {
            buffer[length] = '\n';
            buffer[length + 1] = '\0';
            write(STDOUT_FILENO, "\n", 1);
            length = 0;
        } else if (c < 32 || c == 127) {
            write(STDOUT_FILENO, "\a", 1);
            write(STDOUT_FILENO, "^G", 2);
        } else {
            if (length < MAX_LINE_LEN) {
                buffer[length++] = c;
                write(STDOUT_FILENO, &c, 1);
            } else {
                buffer[length] = '\n';
                buffer[length + 1] = '\0';
                write(STDOUT_FILENO, "\n", 1);
                write(STDOUT_FILENO, &c, 1);
                length = 1;
                buffer[0] = c;
            }
        }
    }

    disable_raw_mode();
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}
