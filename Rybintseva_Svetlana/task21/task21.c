#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define BEEP "\007"

int cnt = 0;

void handle_sigquit(int sig) {
    char buffer[50];  
    int len = snprintf(buffer, sizeof(buffer), "\nBeep sounded: %d times\n", cnt);
    write(STDOUT_FILENO, buffer, len);
    exit(0);
}

void handle_sigint(int sig) {
    write(STDOUT_FILENO, BEEP, 1);
    cnt++;
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
  
    while (1) {
        pause();
    }
    return 0;
}
