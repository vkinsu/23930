#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void handle_sigint(int sig) {
    (void)sig;
    signal_count++;
    printf("\a");
    fflush(stdout);
}

void handle_sigquit(int sig) {
    (void)sig;
    printf("\nThe SIGINT signal has sounded %d times.\n", signal_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    printf("he program has started. Press CTRL-C for beep, CTRL-\\ to exit.\n");

    while (1) {
        pause();
    }

    return 0;
}
