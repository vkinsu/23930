#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

void checkArgs (int argc) {
    if (argc == 1) {
        printf ("Not enough arguments.\n");
        exit (1); 
    }
}

void checkPid (pid_t pid) {
    if (pid == -1) { 
        printf ("Cannot access process id.\n");
        exit (1); 
    }
}

int main(int argc, char** argv) {
    checkArgs (argc);
    
    pid_t pid = fork();
    checkPid (pid);

    if (pid == 0) {
        execvp(argv[1], argv + 1);
        return 1;
    } else {
        int stat_loc;
        wait(&stat_loc);

        if (WIFEXITED(stat_loc)) {
            printf("\nChild process (pid: %d) finished with exit code %d\n",
                   pid, WEXITSTATUS(stat_loc));
        }
    }
    
    return 0;
}