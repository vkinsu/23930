#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
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
    printf("\nTotal: %d\n", signal_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    printf("Control-C - signal, Control-\\ - exit\n");
    while (1) { pause();}
    return 0;
}
