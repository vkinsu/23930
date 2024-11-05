#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    perror("Couldn't make process\n");
    exit(1); 
  }
  if (pid == 0) {
    // Child process
    execlp("cat", "cat", "lab5.c", NULL);
    exit(0);
  } else {
    // Parent process 
    if(wait(NULL) != -1) {
      printf("\nChild process (pid: %d) finished\n", pid);
    }
  }
  exit(0);
}