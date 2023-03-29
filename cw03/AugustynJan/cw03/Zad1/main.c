#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define SUCCESS 0
#define FAILURE 1


int main(int argc, char* argv[]){

    // Arguments validation
    if(argc != 2){
        fprintf(stderr, "Error: Invalid number of arguments\n");
        return FAILURE;
    }
    int numOfProcesses = atoi(argv[1]);
    if (numOfProcesses == 0 && argv[1][0] != '0'){
        fprintf(stderr, "Error: Invalid non-negative integer argument: %s\n", argv[1]);
        return FAILURE;
    }

    // Forking child processes
    pid_t  child_pid;
    while (0 < numOfProcesses--) {
        child_pid = fork();
        if (child_pid == -1){
            perror("Cannot create child process\n");
            exit(FAILURE);
        }
        if (child_pid == 0) {
            printf("PPID: %d PID: %d\n", (int) getppid(), (int) getpid());
            exit(SUCCESS);
        }
    }

    // In Parent process, wait for all child processes to end
    while (wait(NULL) > 0);

    printf("%s\n", argv[1]);
    exit(SUCCESS);
}