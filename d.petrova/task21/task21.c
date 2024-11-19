#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

int sigint_count = 0;

void handle_sigint(int sig) {
	sigint_count++;
    write(STDOUT_FILENO, "\07", 1);
	signal(SIGINT, handle_sigint);
}

void handle_sigquit(int sig) {
    printf("\nSignal worked: %d times\n", sigint_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    printf("Ctrl+C for signal, Ctrl+\\ to quit.\n");
    while (1) {
        pause();
    }

    return 0;
}