#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios old_termios;

void handle_sigquit(int signum) {
    (void)signum;
    // Восстанавливаем старые настройки терминала перед выходом
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    exit(0);
}

int main() {
    struct termios new_termios;

    // Сохраняем старые настройки терминала
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;

    // Отключаем символ прерывания (CTRL-C)
    new_termios.c_cc[VINTR] = _POSIX_VDISABLE;

    // Устанавливаем неканонический режим
    new_termios.c_lflag &= ~(ICANON | ECHO); // Отключаем канонический режим и эхо
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    // Устанавливаем обработчик сигнала SIGQUIT для выхода
    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = handle_sigquit;
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Нажмите CTRL-C для звукового сигнала, CTRL-\\ для выхода.\n");

    while (1) {
        char c = getchar();
        printf("%c", c); // Исправлено на %c

        // Проверяем, является ли введённый символ CTRL-C
        if (c == 3) { // ASCII код для CTRL-C
            ioctl(STDOUT_FILENO, TIOCSBRK, 0);
            printf("Received CTRL-C.\n");
            fflush(stdout);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

    return 0;
}
