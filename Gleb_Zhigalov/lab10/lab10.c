#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    perror("No arguments\n");
    exit(1);
  }
  pid_t pid = fork();
  if (pid == -1) {
    perror("Couldn't make process\n");
    exit(1); 
  }
  if (pid == 0) {
    // Child process
    execvp(argv[1], &argv[1]);
    exit(127);
  } else {
    // Parent process 
    int stat;
    wait(&stat);

    if (WIFEXITED(stat)) {
      printf("\nChild process (pid: %d) finished with exit code %d\n",
              pid, WEXITSTATUS(stat));
    }
  }
  exit(0);
}
