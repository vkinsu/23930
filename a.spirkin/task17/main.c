#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

void disable_echo_canonical(struct termios *original_settings) {
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, original_settings);
    
    new_settings = *original_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

void restore_terminal(struct termios *original_settings) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_settings);
}

int main() {
    struct termios original_settings;
    disable_echo_canonical(&original_settings);

    char input_string[41] = {0};
    int input_len = 0;
    char ch;

    while (1) {
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n <= 0) {
            break;
        }

        switch (ch) {
            case 127: // ERASE
                if (input_len > 0) {
                    input_len--;
                    write(STDOUT_FILENO, "\b \b", 3);
                    input_string[input_len] = '\0';
                }
                break;
            case 21: // KILL
                while (input_len > 0) {
                    write(STDOUT_FILENO, "\b \b", 3);
                    input_len--;
                }
                input_string[0] = '\0';
                break;
            case 4: // CTRL+D
                if (input_len == 0) {
                    restore_terminal(&original_settings);
                    return 0;
                }
                break;
            case 23: // CTRL+W
                if (input_len > 0) {
                    while (input_len > 0 && input_string[input_len - 1] == ' ') {
                        write(STDOUT_FILENO, "\b \b", 3);
                        input_len--;
                    }
                    while (input_len > 0 && input_string[input_len - 1] != ' ') {
                        write(STDOUT_FILENO, "\b \b", 3);
                        input_len--;
                    }
                    input_string[input_len] = '\0';
                }
                break;
            case 10: // \n
                input_string[input_len] = '\n';
                input_string[input_len + 1] = '\0';
                write(STDOUT_FILENO, "\n", 1);
                input_len = 0;
                break;
            default:     
                if (ch < 32 || ch == 127) {
                    write(STDOUT_FILENO, "\a", 1); // BELL
                    write(STDOUT_FILENO, "^G", 2); // CTRL+G
                } else {
                    if (input_len < 40) {
                        input_string[input_len++] = ch;
                        write(STDOUT_FILENO, &ch, 1);
                    } else {
                        input_string[input_len] = '\n';
                        input_string[input_len + 1] = '\0';
                        write(STDOUT_FILENO, "\n", 1);
                        write(STDOUT_FILENO, &ch, 1);
                        input_len = 1;
                        input_string[0] = ch;
                    }
                }
                break;
        }
    }

    restore_terminal(&original_settings);
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}
