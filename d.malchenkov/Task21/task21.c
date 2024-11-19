#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Глобальная переменная для подсчёта сигналов SIGINT
volatile sig_atomic_t sigint_count = 0;

// Обработчик для SIGINT
void handle_sigint(int sig) {
    (void)sig;
    printf("\a"); // Звуковой сигнал (BEL)
    fflush(stdout);
    sigint_count++;
}

// Обработчик для SIGQUIT
void handle_sigquit(int sig) {
    (void)sig;
    printf("\nПрограмма завершена. Количество сигналов SIGINT: %d\n", sigint_count);
    _exit(0);
}

int main() {
    // Установка обработчиков сигналов
    struct sigaction sa_int, sa_quit;

    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = 0;
    sigemptyset(&sa_quit.sa_mask);
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Программа ожидает сигналов. Нажмите Ctrl-C для звукового сигнала, Ctrl-\\ для выхода.\n");
    while (1) {
        pause(); // Ожидание сигнала
    }

    return 0;
}
