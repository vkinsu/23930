#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
    (void)sig;
    sigint_count++;
    printf("\a");
    fflush(stdout); 
}

void handle_sigquit(int sig) {
    (void)sig; 
    printf("\nProgramm ended. Got SIGINT: %d times.\n", sigint_count);
    exit(0);
}

int main() {
    struct sigaction sa_int, sa_quit;
    
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask); 
    
    sa_quit.sa_handler = handle_sigquit;
    sa_quit.sa_flags = 0;
    sigemptyset(&sa_quit.sa_mask); 
    
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        return EXIT_FAILURE;
    }
    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return EXIT_FAILURE;
    }
    
    printf("Programm started. Press Ctrl-C for signal SIGINT or Ctrl-\\ to terminate (SIGQUIT).\n");
    while (1) {
        pause();
    }

    return 0;
}

