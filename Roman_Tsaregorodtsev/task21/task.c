#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int beep_count = 0;

void handle_sigint(int sig) {
    write(1, "\a", 1);
    beep_count++;
}

void handle_sigquit(int sig) {
    printf("\nBeeped %d times.\n", beep_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    while (1) {
        pause();
    }
    return 0;
}
