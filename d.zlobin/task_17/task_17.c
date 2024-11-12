#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_LENGTH 40
#define ERASE 127
#define KILL '\025'
#define CTRL_W '\027'
#define CTRL_D '\004'
#define CTRL_G '\007'

void set_terminal_mode(struct termios *orig_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, orig_termios);
    new_termios = *orig_termios;
    new_termios.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

void restore_terminal_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

void erase_char(char *line, int *length) {
    if (*length > 0) {
        (*length)--;
        line[*length] = '\0';
        printf("\b \b");
        fflush(stdout);
    }
}

void erase_word(char *line, int *length) {
    while (*length > 0 && line[*length - 1] == ' ') {
        erase_char(line, length);
    }
    while (*length > 0 && line[*length - 1] != ' ') {
        erase_char(line, length);
    }
}

int main() {
    struct termios orig_termios;
    set_terminal_mode(&orig_termios);

    char line[MAX_LENGTH + 1];
    int length = 0;
    int c;

    printf("Write a sentence, but only 40 symbols are available *(CTRL-D to end the program)*:\n");

    while (1) {
        c = getchar();

        if (c == CTRL_D && length == 0) {
            break;
        } else if (c == ERASE) {
            erase_char(line, &length);
        } else if (c == KILL) {
            while (length > 0) {
                erase_char(line, &length);
            }
        } else if (c == CTRL_W) {
            erase_word(line, &length);
        } else if (c >= 32 && c <= 126) {
            if (length < MAX_LENGTH) {
                line[length++] = c;
                putchar(c);
                fflush(stdout);
            } else {
                printf("\n");
                fflush(stdout);
                length = 0;
                line[length++] = c;
                putchar(c);
                fflush(stdout);
            }
        } else {
            putchar(CTRL_G);
            fflush(stdout);
        }
    }

    restore_terminal_mode(&orig_termios);
    printf("\nThe end of a program.\n");
    return 0;
}
