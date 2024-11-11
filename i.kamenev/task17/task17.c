#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define ERASE 127        // Символ удаления (Backspace)
#define KILL 21          // Ctrl-U
#define CTRL_W 12        // Ctrl-l
#define CTRL_D 4         // Ctrl-D
#define CTRL_G 7         // Ctrl-G (звуковой сигнал)
#define MAX_LINE_LEN 40  // Максимальная длина строки

void disable_echo_and_canonical(struct termios *old_tio) {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, old_tio);
    new_tio = *old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO | ISIG); // Отключаем канонический режим, эхо и обработку сигналов
    new_tio.c_cc[VMIN] = 1;              // Немедленная обработка каждого символа
    new_tio.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}


void restore_terminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

void erase_character(char *line, int *pos) {
    if (*pos > 0) {
        (*pos)--;
        line[*pos] = '\0';
        write(STDOUT_FILENO, "\b \b", 3); // Удаление символа с экрана
    } else {
        write(STDOUT_FILENO, "\a", 1); // Звуковой сигнал при попытке удалить на пустой позиции
    }
}

void kill_line(char *line, int *pos) {
    while (*pos > 0) {
        (*pos)--;
        write(STDOUT_FILENO, "\b \b", 3); // Удаление символов с экрана
    }
    line[0] = '\0';
}

void erase_word(char *line, int *pos) {
    // Удаляем все пробелы после слова, если они есть
    while (*pos > 0 && line[*pos - 1] == ' ') {
        erase_character(line, pos);
    }

    // Удаляем символы текущего слова до пробела или начала строки
    while (*pos > 0 && line[*pos - 1] != ' ') {
        erase_character(line, pos);
    }

    // Обновляем отображение строки на экране
    write(STDOUT_FILENO, "\r", 1); // Переход к началу строки
    write(STDOUT_FILENO, line, *pos); // Вывод оставшейся части строки
    write(STDOUT_FILENO, "                                       ", MAX_LINE_LEN - *pos); // Очистка остатка
    write(STDOUT_FILENO, "\r", 1); // Снова возвращаемся к началу строки
    write(STDOUT_FILENO, line, *pos); // Вывод строки с удалённым словом
}


int main() {
    struct termios old_tio;
    disable_echo_and_canonical(&old_tio);

    char line[MAX_LINE_LEN + 1] = {0}; // Текущая строка
    int pos = 0;

    printf("Enter text (Ctrl-D to end):\n");

    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == CTRL_D && pos == 0) {
            break; // Завершение при Ctrl-D в начале строки
        } else if (c == ERASE) {
            erase_character(line, &pos);
        } else if (c == KILL) {
            kill_line(line, &pos);
        } else if (c == CTRL_W) {
            erase_word(line, &pos);
        } else if (c >= 32 && c <= 126) { // Проверка печатных символов
            if (pos < MAX_LINE_LEN) {
                line[pos++] = c;
                write(STDOUT_FILENO, &c, 1); // Отображение введенного символа
            } 
            else { 
                // Если достигли конца строки, вставляем перенос строки и начинаем новую строку
                line[pos] = '\0'; // Завершаем текущую строку
                write(STDOUT_FILENO, "\n", 1); // Вставка переноса строки
                pos = 0; // Сброс позиции для новой строки
                line[pos++] = c; // Добавляем символ в новую строку
                write(STDOUT_FILENO, &c, 1); // Отображение символа в новой строке
    }
}
 else {
            write(STDOUT_FILENO, "\a", 1); // Звуковой сигнал для непечатаемых символов
        }
    }

    restore_terminal(&old_tio);
    write(STDOUT_FILENO, "\nProgram terminated.\n", 23);
    return 0;
}

