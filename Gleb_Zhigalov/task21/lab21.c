#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int cnt = 0;

void handle_signal(int sig) {
  if (sig == SIGQUIT) {
    char buffer[50];  
    int len = snprintf(buffer, sizeof(buffer), "\nBeep sounded: %d times\n", cnt);
    write(STDOUT_FILENO, buffer, len);
    exit(0);
  }
  else if (sig == SIGINT) {
    write(STDOUT_FILENO, "\007", 1);
    cnt++;
    signal(SIGINT, handle_signal);
  }
}

int main() {
  signal(SIGINT, handle_signal);
  signal(SIGQUIT, handle_signal);
  
  while (1) {
    pause();
  }
  return 0;
}