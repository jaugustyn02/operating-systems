#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void sigusr1_handler(int sig_no){
    printf("Received signal SIGUSR1 (%d)\n", sig_no);
    fflush(NULL);
}

void test_ignore(){
    signal(SIGUSR1, SIG_IGN);       // ignore signal SIGUSR1

    printf("PID: %d - raising SIGUSR1\n", (int)getpid());
    fflush(NULL);
    raise(SIGUSR1);

    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("Could not create child process\n");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        printf("PID: %d - raising SIGUSR1\n", (int)getpid());
        raise(SIGUSR1);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
}

void test_handler(){
    signal(SIGUSR1, sigusr1_handler);       // setting custom handler for SIGUSR1

    printf("PID: %d - raising SIGUSR1\n", (int)getpid());
    fflush(NULL);
    raise(SIGUSR1);

    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("Could not create child process\n");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        printf("PID: %d - raising SIGUSR1\n", (int)getpid());
        raise(SIGUSR1);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
}

void test_mask(){
    struct sigaction act;
    sigemptyset(&act.sa_mask);                      // init empty set of signals
    sigaddset(&act.sa_mask, SIGUSR1);               // adding SIGUSR1 to set
    sigprocmask(SIG_BLOCK, &act.sa_mask, NULL);     // blocking all signals in set

    printf("PID: %d - raising SIGUSR1\n", (int)getpid());
    fflush(NULL);
    raise(SIGUSR1);

    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("Could not create child process\n");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        printf("PID: %d - raising SIGUSR1\n", (int)getpid());
        raise(SIGUSR1);
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
}

void test_pending(){
    // mask
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("PID: %d - raising SIGUSR1\n", (int)getpid());
    fflush(NULL);
    raise(SIGUSR1);
    // pending
    sigpending(&set);
    if (sigismember(&set, SIGUSR1)) {
        printf("PID: %d - signal SIGUSR1 is pending\n", (int)getpid());
    }
    else {
        printf("PID: %d - Signal SIGUSR1 is not pending\n", (int)getpid());
    }
    fflush(NULL);

    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("Could not create child process\n");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0) {
        sigpending(&set);
        if (sigismember(&set, SIGUSR1)) {
            printf("PID: %d - signal SIGUSR1 is pending\n", (int)getpid());
        }
        else {
            printf("PID: %d - Signal SIGUSR1 is not pending\n", (int)getpid());
        }
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s <ignore|handler|mask|pending>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "ignore") == 0) {
        printf("Testing ignore:\n");
        test_ignore();
    } else if (strcmp(argv[1], "handler") == 0) {
        printf("Testing handler:\n");
        test_handler();
    } else if (strcmp(argv[1], "mask") == 0) {
        printf("Testing mask:\n");
        test_mask();
    } else if (strcmp(argv[1], "pending") == 0) {
        printf("Testing pending:\n");
        test_pending();
    } else {
        printf("Error: Invalid argument: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    return 0;
}
