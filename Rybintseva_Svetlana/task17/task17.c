#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 40
#define KIIL_STR "\r\033[K"
#define ERASE_C "\b \b"
#define ENTER_CH "\n"
#define BEEP "\x07"

int main() {
    struct termios oldt, newt;
    char input[MAX_INPUT];

    // Получаем текущие параметры терминала
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;

    int index = 0;
    int cnt = 0;

    // Отключаем канонический режим и эхо
    // newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);

    // Устанавливаем управляющие символы
    newt.c_cc[VERASE] = 8;   // Backspace
    newt.c_cc[VWERASE] = 23; // Ctrl+W
    newt.c_cc[VKILL] = 21;   // Ctrl+U
    newt.c_cc[VEOF] = 4;     // Ctrl+D

    while (1) {
        char ch;
        read(STDIN_FILENO, &ch, 1);

        switch (ch) {
            case 10: // Enter
                index = 0;
                write(1, &ch, 1);
                break;

            case 4: // Ctrl-D
                if (index == 0) {
                    goto end; // Выход из цикла
                }
                break;

            case 21: // Ctrl-U (KILL)
                write(1, KIIL_STR, strlen(KIIL_STR));
                index = 0;
                continue;

            case 8: // Backspace
            case 127: // Delete
                if (index > 0) {
                    index--;
                    write(1, ERASE_C, strlen(ERASE_C));
                }
                continue;

            case 23: // Ctrl-W (VWERASE)
                while (index > 0 && input[index - 1] == ' ') {
                    index--; // Удаляем пробелы
                    write(1, ERASE_C, strlen(ERASE_C));
                }
                while (index > 0 && input[index - 1] != ' ') {
                    index--;
                    write(1, ERASE_C, strlen(ERASE_C));
                }
                continue;

            default:
                if (!isprint(ch)){
                    write(STDOUT_FILENO, BEEP, strlen(BEEP));
                    continue;
                }
                // Выводим символ и сохраняем его в массив
                else if (index < MAX_INPUT - 1) {
                    input[index++] = ch;
                    write(1, &ch, 1);
                } else {
                    index = 0;
                    write(1, ENTER_CH, 1);
                    input[index++] = ch;
                    write(1, &ch, 1);
                }
                break;
        }
    }

end:
    // Восстанавливаем старые параметры
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    return 0;
}
