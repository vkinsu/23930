#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define ERASE 127        // Символ удаления
#define KILL 21          // Ctrl-U
#define CTRL_W 23        // Ctrl-W
#define CTRL_D 4         // Ctrl-D
#define CTRL_G 7         // Ctrl-G (звуковой сигнал)
#define MAX_LINE_LEN 40  // Максимальная длина строки

void disable_echo_and_canonical(struct termios *old_tio) {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, old_tio);
    new_tio = *old_tio;
    new_tio.c_lflag &= ~ICANON; 
    new_tio.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_terminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

void erase_character(char *line, int *pos) {
    if (*pos > 0) {
        (*pos)--;
        line[*pos] = '\0';
        write(STDOUT_FILENO, "\b \b", 3);
    } else {
        write(STDOUT_FILENO, "\a", 1);
    }
}

void kill_line(char *line, int *pos) {
    while (*pos > 0) {
        (*pos)--;
        write(STDOUT_FILENO, "\b \b", 3);
    }
    line[0] = '\0';
}

void erase_word(char *line, int *pos) {
    while (*pos > 0 && line[*pos - 1] == ' ') {
        erase_character(line, pos);
    }
    while (*pos > 0 && line[*pos - 1] != ' ') {
        erase_character(line, pos);
    }
}

int main() {
    struct termios old_tio;
    disable_echo_and_canonical(&old_tio);

    char line[MAX_LINE_LEN + 1] = {0}; 
    int pos = 0;

    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == CTRL_D && pos == 0) 
            break;
        else if (c == ERASE) 
            erase_character(line, &pos);
        else if (c == KILL) 
            kill_line(line, &pos);
        else if (c == CTRL_W) 
            erase_word(line, &pos);
        else if (c >= 32 && c <= 126) { 
            if (pos < MAX_LINE_LEN) {
                line[pos++] = c;
                write(STDOUT_FILENO, &c, 1);
                if (pos == MAX_LINE_LEN) {
                    write(STDOUT_FILENO, "\n", 1);
                    pos = 0;
                }
            } 
            else 
                write(STDOUT_FILENO, "\a", 1);
        }
        else 
            write(STDOUT_FILENO, "\a", 1);
    }

    restore_terminal(&old_tio);
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}